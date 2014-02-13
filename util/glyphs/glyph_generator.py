import os
import json
import datetime

# Define the state of each LED, where a 1 is on and 0 is off
alphabet = [ \
    { 'char': [' '],
      'symb': [ [ 0, 0, 0, 0, 0 ], \
                [ 0, 0, 0, 0, 0 ], \
                [ 0, 0, 0, 0, 0 ], \
                [ 0, 0, 0, 0, 0 ], \
                [ 0, 0, 0, 0, 0 ], \
                [ 0, 0, 0, 0, 0 ], \
                [ 0, 0, 0, 0, 0 ], \
                [ 0, 0, 0, 0, 0 ] ]\
    }, \
    { 'char': ['A', 'a'],
      'symb': [ [ 1, 1, 1, 1, 1 ], \
                [ 1, 0, 0, 0, 1 ], \
                [ 1, 0, 0, 0, 1 ], \
                [ 1, 1, 1, 1, 1 ], \
                [ 1, 0, 0, 0, 1 ], \
                [ 1, 0, 0, 0, 1 ], \
                [ 1, 0, 0, 0, 1 ], \
                [ 1, 0, 0, 0, 1 ] ]\
    }, \
    { 'char': ['R', 'r'],
      'symb': [ [ 1, 1, 1, 1, 1 ], \
                [ 1, 0, 0, 0, 1 ], \
                [ 1, 0, 0, 0, 1 ], \
                [ 1, 1, 1, 1, 1 ], \
                [ 1, 1, 0, 0, 0 ], \
                [ 1, 0, 1, 0, 0 ], \
                [ 1, 0, 0, 1, 0 ], \
                [ 1, 0, 0, 0, 1 ] ]\
    } ]

led_key  = [ 6, 7, 0, 1, 2, 3, 4, 5 ]

def DeleteFile(filename):
    """Delete a file if it exists"""
    try:
        os.remove(filename)
    except OSError:
        pass


def WriteHeader(f, name, description=None):
    datestr = datetime.date.today().strftime('%d %b %Y')

    f.write('/**\n')
    f.write(' * @file %s\n' % name)
    f.write(' *\n')
    f.write(' * @author This file was procedurally generated on %s\n' % datestr)
    if description != None:
        f.write(' *\n' + description + '\n')
    f.write(' */\n\n')


root_name       = 'GLYPH'
start_idx_arr   = root_name + '_IDX'
width_arr       = root_name + '_WIDTH'
data_arr        = root_name + '_DATA'


header_description = \
' * Define the glyphs used to display a message.\n' +\
' *\n' +\
' * Using an 8-LED display, each column of a glyph is defined in byte\n' +\
' * in which the LSB corresponds to the state of the LED on the bottom\n' +\
' * row of the glyph, and each subsequent bit corresponds to each\n' +\
' * subsequent LED. A value of 0 indicates the LED should be on, while\n' +\
' * a value of 1 indicates the LED should be off.\n' +\
' *\n' +\
' * To determine the representation of a given glyph, find the start index\n' +\
' * of its data in %s, then the number of bytes of data (its width)\n' % (start_idx_arr) +\
' * in %s, then read the correct number of bytes from %s.' % (width_arr, data_arr)



if __name__ == '__main__':
    config_file = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'config.json')
    with open(config_file, 'rb') as f:
        config = json.load(f)
        glyph_name  = config['glyph_name']
        glyph_path  = config['glyph_path']
            
    header_name = glyph_name + '.h'
    header_grd  = glyph_name.upper() + '_H_'
    DeleteFile(os.path.join(glyph_path,header_name))
    with open(os.path.join(glyph_path, header_name), 'wb') as f:
        WriteHeader(f, header_name, header_description)
        f.write('#ifndef %s\n' % header_grd) 
        f.write('#define %s\n\n' % header_grd) 
        f.write('#include <stdint.h>\n')
        f.write('#include <avr/pgmspace.h>\n\n')
        f.write('enum {\n')
        f.write('    /// The number of defined glyphs\n')
        f.write('    GLYPH_COUNT = 0x%02x,\n' % len(alphabet))
        f.write('};\n\n')
        f.write("/// The start index of a glyph's data in %s\n" % data_arr)
        f.write('extern const uint8_t %s[] PROGMEM;\n\n' % start_idx_arr) 
        f.write('/// The number of bytes of data for the given glyph\n')
        f.write('extern const uint8_t %s[] PROGMEM;\n\n' % width_arr)
        f.write('/// Glyph data\n')
        f.write('extern const uint8_t %s[] PROGMEM;\n\n' % data_arr)
        f.write('#endif // %s' % header_grd)

    widths = []
    starts = [ 0 ]
    for a in alphabet:
        widths.append(len(a['symb'][0]))
        starts.append(starts[-1] + widths[-1])

    source_name = glyph_name + '.c'
    DeleteFile(os.path.join(glyph_path, source_name))
    with open(os.path.join(glyph_path, source_name), 'wb') as f:
        WriteHeader(f, source_name)
        f.write('#include "%s"\n\n' % header_name)

        f.write('const uint8_t %s[] PROGMEM = {' % start_idx_arr)
        for i in range(0, len(alphabet)-1):
            if (i % 10) == 0:
                f.write('\n    ')
            f.write('0x%02x, ' % starts[i])
        f.write('0x%02x\n};\n\n' % starts[len(alphabet)-1])

        f.write('const uint8_t %s[] PROGMEM = {' % width_arr)
        for i in range(0, len(alphabet)-1):
            if (i % 10) == 0:
                f.write('\n    ')
            f.write('0x%02x, ' % widths[i])
        f.write('0x%02x\n};\n\n' % widths[len(alphabet)-1])

        f.write('const uint8_t %s[] PROGMEM = {' % data_arr)
        for a in alphabet:
            f.write('\n    ')
            for c in range(0, len(a['symb'][0])):
                f.write('0b')
                for led in led_key:
                    bit = (a['symb'][led][c] + 1) % 2
                    f.write('%d' % bit)
                if a == alphabet[-1] and c == (len(a['symb'][0])-1):
                    f.write('\n};')
                else:
                    f.write(', ')
