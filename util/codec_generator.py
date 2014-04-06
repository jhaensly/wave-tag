import twitter
import pickle
import os
import unicodedata
import Queue
import copy
import plistlib
import datetime
import pdb
import json
from collections import deque

alphabet = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
            'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
           ]


def DeleteFile(filename):
    """Delete a file if it exists"""
    try:
        os.remove(filename)
    except OSError:
        pass


def GetTwitterApi(filename):
    """Log into the twitter using the credentials saved in filename.
    The credentials should be a dictionary with the keys 'consumer_key',
    'consumer_secret', 'access_token', and 'access_secret' that's been
    pickled."""

    with open(filename, 'r') as f:
        c = pickle.load(f)
        return twitter.Api( \
            c.get('consumer_key'), \
            c.get('consumer_secret'), \
            c.get('access_token'), \
            c.get('access_secret'))


def GetTweetsFromTwitter(count, filename):
    """Get some number of English-language tweets from Twitter."""

    api = GetTwitterApi(filename)
    c = count
    while 1:
        try:
            for stream in api.GetStreamSample():
                if stream.get('lang') == 'en':
                    yield stream.get('text')
                    c -= 1

                if c <= 0:
                    break
            if c <= 0:
                break
        except:
            pass
            

class CharacterProbability:
    def __init__(self):
        self.Reset()

    def Reset(self):
        self.d = dict()
        self.n = 0

    def Add(self, c):
        self.n += 1
        if c in self.d:
            self.d[c] += 1
        else:
            self.d[c] = 1

    def GetFreq(self, c):
        if c in self.d:
            return self.d[c]
        else:
            return 0

    def GetProb(self, c):
        if c in self.d:
            return self.d[c] / self.n
        else:
            return 0

    def Save(self, filename):
        with open(filename, 'wb') as f:
            pickle.dump([self.d, self.n], f)

    def Load(self, filename):
        with open(filename, 'r') as f:
            self.d, self.n = pickle.load(f)



LEFT  = 0
RIGHT = 1
class HuffmanTree:
    class Node:
        def __init__(self, left=None, right=None):
            self.left  = left
            self.right = right
            self.nodeCount = 0

    def __init__(self, frequencies):
        # Sort the alphabet by probability. A PriorityQueue always
        # returns the lowest valued entries first
        q = Queue.PriorityQueue()
        for f in frequencies:
            q.put(f)

        # While there is more than one node, remove the two lowest
        # probability nodes, create an internal node with children,
        # and add a new node to the queue
        while q.qsize() > 1:
            l, r = q.get(), q.get()
            n = self.Node(l,r)
            q.put((l[0]+r[0], n))

        self.root = q.get()

    def _GetNode(self, node):
        return node[1]

    def _GetRoot(self):
        return self._GetNode(self.root)

    def _Walk(self, node, path):
        if isinstance(node, self.Node):
            path.append(LEFT)
            for n, p in self._Walk(self._GetNode(node.left), path):
                yield n, p
            path.pop()

            path.append(RIGHT)
            for n, p in self._Walk(self._GetNode(node.right), path):
                yield n, p
            path.pop()
        else:
            yield node, path

    def Walk(self):
        for n, p in self._Walk(self._GetRoot(), []):
            yield n, p

    def Serialize(self, junk=255):
        idx = 0
        arr = [junk, junk]
        q = deque([self._GetNode(self._GetRoot().left), self._GetNode(self._GetRoot().right)])

        while len(q) > 0:
            n = q.popleft()
            if isinstance(n, self.Node):
                arr[idx] = len(arr)
                arr.append(junk)
                arr.append(junk)

                q.append(self._GetNode(n.left))
                q.append(self._GetNode(n.right))
            elif n is not None:
                arr[idx] = alphabet.index(n) + 1 + 128
            idx += 1

        #pdb.set_trace()
        return arr

if __name__ == '__main__':
    config_file = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'codec_config.json')
    with open(config_file, 'r') as f:
        config = json.load(f)
        tweets_file   = os.path.join(\
                        config['tweets_path'],\
                        config['tweets_name'] + config['tweets_ext'])
        char_file     = os.path.join(\
                        config['char_path'],
                        config['char_name'] + config['char_ext'])
        encoder_file  = os.path.join(\
                        config['encoder_path'],\
                        config['encoder_name']) 
        decoder_name  = config['decoder_name']
        decoder_file  = os.path.join(\
                        config['decoder_path'],\
                        decoder_name)
        twitter_cred  = os.path.join(config['twitter_credentials_path'],\
                        config['twitter_credentials_name'] + config['twitter_credentials_ext'])

        refresh_tweets = config['refresh_tweets']

    p = CharacterProbability()

    if refresh_tweets:
        DeleteFile(tweets_file)

        with open(tweets_file, 'wb') as f:
            count = 0
            target = 1000000
            for tweet in GetTweetsFromTwitter(target, twitter_cred):
                count += 1
                if (count % 100) == 0:
                    print 'Tweet #%d' % count

                # write the original tweet to file
                f.write(tweet.encode('utf8'))
                f.write('\n\n')

                # convert unicode tweets to ASCII
                a = unicodedata.normalize('NFKD',tweet).encode('ascii','ignore') 

                # update the CharacterProbability object with new data
                for c in a:
                    p.Add(c.lower())

        p.Save(char_file)

    else:
        p.Load(char_file)

    # Create and populate a Huffman tree
    freq = []
    for a in alphabet:
        freq.append((p.GetFreq(a), a))
    h = HuffmanTree(freq)

    # Generate the encoder dictionary
    encoder_dictionary = {}
    for symbol, code in h.Walk():
        encoder_dictionary[symbol] = copy.deepcopy(code)

    # Write the dictionary to a property list (plist) file that can be
    # imported from iOS/OSX
    plistlib.writePlist(encoder_dictionary, encoder_file+ '.plist')

    garbage_value = 255
    arr = h.Serialize(garbage_value)

    datestr = datetime.date.today().strftime('%d %b %Y')
    yearstr = datetime.date.today().strftime('%Y')

    DeleteFile(decoder_file + '.h')
    DeleteFile(decoder_file + '.c')

    arr_name  = decoder_name.upper()
    numel_name   = arr_name + '_ELEMENTS'
    invalid_name = arr_name + '_INVALID'
    header_grd   = arr_name.upper() + '_H_'

    with open(decoder_file + '.h', 'wb') as f:
        f.write('/**\n')
        f.write(' * @file %s.h\n' % decoder_name)
        f.write(' *\n')
        f.write(' * @author This file was procedurally generated on %s\n' % datestr)
        f.write(' *\n')
        f.write(' * @copyright Copyright (c) %s Blinc Labs LLC\n' % yearstr)
        f.write(' * @copyright This software is licensed under the terms and conditions of the\n')
        f.write(' * MIT License. See LICENSE.md in the root directory for more information.\n')
        f.write(' *\n')
        f.write(' * The decoding scheme for the Huffman encoded data used for VLC. The value at\n')
        f.write(' * an index represents either the index in the array of its left child or, if\n')
        f.write(' * the most significant bit is set, the value itself. The index of the right\n')
        f.write(' * child is the stored index value + 1. If there is no right or left child, the\n')
        f.write(' * value at that index will equal %s\n' % invalid_name)
        f.write(' */\n\n')
        f.write('#ifndef %s\n' % header_grd)
        f.write('#define %s\n\n' % header_grd)
        f.write('#include <stdint.h>\n')
        f.write('#include <avr/pgmspace.h>\n\n')
        f.write('enum {\n')
        f.write('    /** The number of elements in the matrix */\n')
        f.write('    %s = %d,\n\n' % (numel_name, len(arr)))
        f.write('    /** The value used to denote invalid symbols */\n')
        f.write('    %s  = 0x%02x,\n' % (invalid_name, garbage_value))
        f.write('};\n\n')
        f.write('/** The decoder matrix */\n')
        f.write('extern const uint8_t %s[%s] PROGMEM;\n\n\n' % (arr_name, numel_name))
        f.write('#endif // %s' % header_grd)

    with open(decoder_file + '.c', 'wb') as f:
        f.write('/**\n')
        f.write(' * @file %s.c\n' % decoder_name)
        f.write(' *\n')
        f.write(' * @author This file was procedurally generated on %s\n' % datestr)
        f.write(' *\n')
        f.write(' * @copyright Copyright (c) %s Blinc Labs LLC\n' % yearstr)
        f.write(' * @copyright This software is licensed under the terms and conditions of the\n')
        f.write(' * MIT License. See LICENSE.md in the root directory for more information.\n')
        f.write(' *\n')
        f.write(' */\n\n')
        f.write('#include "%s.h"\n\n' % decoder_name)
        f.write('const uint8_t %s[%s] PROGMEM = {' % (arr_name, numel_name))
        for i in range(0, len(arr)-1):
            if (i % 10) == 0:
                f.write('\n    ')
            f.write('0x%02x, ' % arr[i])
        f.write('0x%02x };' % arr[len(arr)-1])
