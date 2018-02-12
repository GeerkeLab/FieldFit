#! /bin/bash

import json
import sys
import os
import numpy
import scipy
import argparse
import scipy.stats
from copy import deepcopy
from collections import OrderedDict

def BaseName(path):

    base = os.path.basename(path)
    #filename, file_extension = os.path.splitext(base)

    return base

def LoadFieldfitData( ifs ):

    return json.load(ifs)

def CollectData( datacache ):

    per_compound_rmsd  = OrderedDict()
    per_compound_group = OrderedDict()
    per_key_group = OrderedDict()
    site_to_key = {}

    for data_file, data in datacache.items():

        fits = data["fits"]
        for system, system_data in fits.items():
            rmsd = system_data["rmsd"]
            sites = system_data["sites"]

            # create system in compound group
            if not system in per_compound_rmsd:
                per_compound_rmsd[system] = []

            per_compound_rmsd[system].extend(rmsd)

            if not system in per_compound_group:
                per_compound_group[system] = OrderedDict()

            compound_group = per_compound_group[system]

            for fit_site, fit_site_data in sites.items():
                fit_keys = fit_site_data["fit_keys"]
                fit_result = fit_site_data["fit_result"]

                # FOR NOW ONLY USE THE FIRST KEY!
                first_key = fit_keys[0]

                site_to_key_index = "%s::%s" % ( system, fit_site)
                site_to_key[site_to_key_index] = first_key

                # create site if not present
                if not fit_site in compound_group:
                    compound_group[fit_site] = OrderedDict()

                # create key in key group
                if not first_key in per_key_group:
                    per_key_group[first_key] = OrderedDict()

                key_group = per_key_group[first_key]

                for fit_class, fit_class_data in fit_result.items():

                    # add fit_classes if needed
                    if not fit_class in compound_group[fit_site]:
                        compound_group[fit_site][fit_class] = deepcopy(fit_class_data)
                    else:
                        compound_group[fit_site][fit_class].extend(fit_class_data)

                    if not fit_class in key_group:
                        key_group[fit_class] = deepcopy(fit_class_data)
                    else:
                        key_group[fit_class].extend(fit_class_data) 
    
    return per_compound_group, per_key_group, site_to_key, per_compound_rmsd

def PerCompoundStats( per_compound_group, target_fit_class, site_to_key, winsorize ):

    total_charges = {}
    total_polarizabilities = {}

    out_str=""
    for compound, compound_data in per_compound_group.items():
        for site, site_data in compound_data.items():

            site_to_key_index = "%s::%s" % ( compound, site)
            constr_key = site_to_key[site_to_key_index]

            for fit_class, fit_class_data in site_data.items():
                if target_fit_class is None or fit_class == target_fit_class:

                    avg    = numpy.average(fit_class_data)
                    median = numpy.median(fit_class_data)
                    stdev  = numpy.std(fit_class_data)
                    
                    if winsorize:
                        winsor = scipy.stats.mstats.winsorize(fit_class_data, (0.05, 0.05))
                        avg    = numpy.average(winsor)
                        median = numpy.median(winsor)
                        stdev  = numpy.std(winsor)

                    
                    if target_fit_class == "charge":
                        if not compound in total_charges:
                            total_charges[compound] = 0.0
                        total_charges[compound] += avg

                    if target_fit_class == "alpha":
                        if not compound in total_polarizabilities:
                            total_polarizabilities[compound] = 0.0
                        total_polarizabilities[compound] += avg


                    out_str+= "%s %s %s %s %f %f %f \n" % ( compound, site, constr_key, fit_class, avg, median, stdev)

    for compound, value in total_charges.items():
        out_str+= "total_charges %s %f \n" % ( compound, value)

    for compound, value in total_polarizabilities.items():
        out_str+= "total_polarizabilities %s %f \n" % ( compound, value)

    print (out_str)

def PerKeyStats( per_key_group, target_fit_class ):

    out_str=""

    for key, key_data in per_key_group.items():

        for fit_class, fit_class_data in key_data.items():

            if target_fit_class is None or fit_class == target_fit_class:

                N      = len(fit_class_data)
                avg    = numpy.average(fit_class_data)
                median = numpy.median(fit_class_data)
                stdev  = numpy.std(fit_class_data)
                
                if winsorize:
                    winsor = scipy.stats.mstats.winsorize(fit_class_data, (0.05, 0.05))
                    avg    = numpy.average(winsor)
                    median = numpy.median(winsor)
                    stdev  = numpy.std(winsor)

                out_str+= "%s %s %i %f %f %f \n" % ( key, fit_class, N, avg, median, stdev)

    print (out_str)

def PerCompoundRmsd( per_compound_rmsd ):

    for compound_key, rmsds in per_compound_rmsd.items():
        
        rmsds_2 = numpy.square( rmsds )
        total_sum = rmsds_2.mean()
        total_rmsd = numpy.sqrt( total_sum )
        
        print( "%s %f" % ( compound_key, total_rmsd ) )

def main( argv ):

    parser = argparse.ArgumentParser(description='Gather stats from field fit files')
    parser.add_argument('strings', metavar='N', type=str, nargs='+',
                         help='fieldfit files')
    parser.add_argument('--fit_class', nargs='?', help='select specific fit class')
    parser.add_argument('--winsorize', help='winsorize distributions', action='store_true')

    args = parser.parse_args()

    data_objs = {}
    for path in args.strings:
        coll_name = BaseName(path)

        print ( "coll_name %s" % (coll_name) )
        with open(path,'r') as ifs:
            data_objs[coll_name] = LoadFieldfitData(ifs)
    
    per_compound_group, per_key_group, site_to_key, per_compound_rmsd = CollectData(data_objs)

    PerCompoundStats( per_compound_group, args.fit_class, site_to_key, args.winsorize )
    PerKeyStats( per_key_group, args.fit_class, winsorize )
    PerCompoundRmsd( per_compound_rmsd )

if __name__ == "__main__":
    # execute only if run as a script
    main(sys.argv[1:])


