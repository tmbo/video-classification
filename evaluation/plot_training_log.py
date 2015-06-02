#!/usr/bin/env python
import inspect
import os
import random
import sys
import matplotlib.cm as cmx
import matplotlib.colors as colors
import matplotlib.pyplot as plt
import matplotlib.legend as lgd
import matplotlib.markers as mks

def get_log_parsing_script():
    dirname = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))
    return dirname + '/parse_log.sh'

def get_log_file_suffix():
    return '.log'

def get_chart_type_description_separator():
    return '  vs. '

def is_x_axis_field(field):
    x_axis_fields = ['Iters', 'Seconds']
    return field in x_axis_fields

def create_field_index():
    train_key = 'Train'
    test_key = 'Test'
    field_index = {train_key:{'Iters':0, 'Seconds':1, train_key + ' loss':2,
                              train_key + ' learning rate':3},
                   test_key:{'Iters':0, 'Seconds':1, test_key + ' accuracy':2,
                             test_key + ' loss':3}}
    fields = set()
    for data_file_type in field_index.keys():
        fields = fields.union(set(field_index[data_file_type].keys()))
    fields = list(fields)
    fields.sort()
    return field_index, fields

def get_supported_chart_types():
    field_index, fields = create_field_index()
    num_fields = len(fields)
    supported_chart_types = []
    for i in xrange(num_fields):
        if not is_x_axis_field(fields[i]):
            for j in xrange(num_fields):
                if i != j and is_x_axis_field(fields[j]):
                    supported_chart_types.append('%s%s%s' % (
                        fields[i], get_chart_type_description_separator(),
                        fields[j]))
    return supported_chart_types

def get_chart_type_description(chart_type):
    supported_chart_types = get_supported_chart_types()
    chart_type_description = supported_chart_types[chart_type]
    return chart_type_description

def get_data_file_type(chart_type):
    description = get_chart_type_description(chart_type)
    data_file_type = description.split()[0]
    return data_file_type

def get_data_file(chart_type, path_to_log):
    return os.path.basename(path_to_log) + '.' + get_data_file_type(chart_type).lower()

def get_field_descriptions(chart_type):
    description = get_chart_type_description(chart_type).split(
        get_chart_type_description_separator())
    y_axis_field = description[0]
    x_axis_field = description[1]
    return x_axis_field, y_axis_field    

def get_field_indecies(x_axis_field, y_axis_field):    
    data_file_type = get_data_file_type(chart_type)
    fields = create_field_index()[0][data_file_type]
    return fields[x_axis_field], fields[y_axis_field]

def load_data(data_file, field_idx0, field_idx1):
    data = [[], []]
    with open(data_file, 'r') as f:
        for line in f:
            line = line.strip()
            if line[0] != '#':
                fields = line.split()
                data[0].append(float(fields[field_idx0].strip()))
                data[1].append(float(fields[field_idx1].strip()))
    return data

def random_marker():
    markers = mks.MarkerStyle.markers
    num = len(markers.values())
    idx = random.randint(0, num - 1)
    return markers.values()[idx]

def get_data_label(path_to_log):
    label = path_to_log[path_to_log.rfind('/')+1 : path_to_log.rfind(
        get_log_file_suffix())]
    return label

def get_legend_loc(chart_type):
    x_axis, y_axis = get_field_descriptions(chart_type)
    loc = 'lower right'
    if y_axis.find('accuracy') != -1:
        pass
    if y_axis.find('loss') != -1 or y_axis.find('learning rate') != -1:
        loc = 'upper right'
    return loc

def plot_chart(path_to_out_dir, path_to_log_list):
    for chart_type in (0 : len(get_supported_chart_types())):
        for path_to_log in path_to_log_list:
            os.system('%s %s' % (get_log_parsing_script(), path_to_log))
            data_file = get_data_file(chart_type, path_to_log)
            x_axis_field, y_axis_field = get_field_descriptions(chart_type)
            x, y = get_field_indecies(x_axis_field, y_axis_field)
            data = load_data(data_file, x, y)
            ## TODO: more systematic color cycle for lines
            color = [random.random(), random.random(), random.random()]
            label = get_data_label(path_to_log)
            linewidth = 0.75
            ## If there too many datapoints, do not use marker.
    ##        use_marker = False
            use_marker = True
            if not use_marker:
                plt.plot(data[0], data[1], label = label, color = color,
                         linewidth = linewidth)
            else:
                ok = False
                ## Some markers throw ValueError: Unrecognized marker style
                while not ok:
                    try:
                        marker = random_marker()
                        plt.plot(data[0], data[1], label = label, color = color,
                                 marker = marker, linewidth = linewidth)
                        ok = True
                    except:
                        pass
        legend_loc = get_legend_loc(chart_type)
        plt.legend(loc = legend_loc, ncol = 1) # ajust ncol to fit the space
        plt.title(get_chart_type_description(chart_type))
        plt.xlabel(x_axis_field)
        plt.ylabel(y_axis_field)  
        plt.savefig("%s/%s.png" % (
            path_to_out_dir, 
            get_chart_type_description(chart_type).replace(" ", "_"))
        plt.show()

def print_help():
    print """Usage:
    ./plot_log.sh /where/to/save_dir /path/to/first.log ...
Notes:
    1. Supporting multiple logs.
    2. Log file name must end with the lower-cased "%s".
Supported chart types:""" % get_log_file_suffix()
    for typ in get_supported_chart_types():
        print '    %s' % typ
    exit

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print_help()
    else:
        path_to_out_dir = sys.argv[1]
        if not os.path.exists(path_to_out_dir):
            print 'Output path does not exist: %s' % path_to_out_dir
            exit
        path_to_logs = sys.argv[2:]
        for path_to_log in path_to_logs:
            if not os.path.exists(path_to_log):
                print 'Path does not exist: %s' % path_to_log
                exit
            if not path_to_log.endswith(get_log_file_suffix()):
                print_help()
                exit
        ## plot_chart accpets multiple path_to_logs
        plot_chart(path_to_out_dir, path_to_logs)
