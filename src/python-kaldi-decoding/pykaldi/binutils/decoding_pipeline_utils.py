import os
from ordereddefaultdict import DefaultOrderedDict
import argparse
import errno
import json


def parse_config_from_arguments():
    parser = argparse.ArgumentParser()
    parser.add_argument("config", help='The main config file')
    args = parser.parse_args()
    with open(args.config, 'r') as r:
        config = json.load(r)
    # Replace {'prefix':'key_to_path', 'value':'suffix_of_path'} with correct path
    config = expand_prefix(config, config)
    return config


class PyKaldiError(Exception):
    def __init__(self, retcode):
        self.retcode = retcode

    def __str__(self):
        return 'Failed with return code: %s' % repr(self.retcode)


def make_dir(path):
    try:
        os.makedirs(path)
    except OSError as exception:
        if exception.errno != errno.EEXIST:
            raise


def expand_prefix(d, bigd):
    ''' Replace {'prefix':'key_to_path', 'value':'suffix_of_path'} with correct path'''
    if isinstance(d, dict):
        if len(d) == 2 and 'prefix' in d and 'value' in d:
            s = os.sep.join([bigd[d['prefix']], d['value']])
            return s.encode('utf-8')
        else:
            for k, v in d.iteritems():
                d[k] = expand_prefix(v, bigd)
            return d
    elif isinstance(d, list):
        return [expand_prefix(x, bigd) for x in d]
    elif isinstance(d, unicode):
        # we need strings not unicode
        return d.encode('utf-8')
    else:
        raise ValueError('We support only dictionaries, lists and strings in config')


def build_reference(wav_scp, ref_path):
    with open(ref_path, 'w') as w:
        with open(wav_scp, 'r') as scp:
            for line in scp:
                name, wavpath = line.strip().split(' ', 1)
                with open(wavpath + '.trn') as trn:
                    trans = trn.read().strip()
                    w.write('%s %s\n' % (name, trans))


def wst2dict(wst_path):
    ''' Stores word symbol table (WST) like dictionary.
    The numbers are stored like string values
    Example line of WST looks like:
    sample_word  1234
    '''
    with open(wst_path, 'r') as r:
        # split removes empty and white space only splits
        line_arr = [line.split() for line in r.readlines()]
        d = dict([])
        for arr in line_arr:
            assert len(arr) == 2, 'Word Symbol Table should have 2 records on each row'
            # WST format:  WORD  NUMBER  ...we store d[NUMBER] = WORD
            d[arr[1]] = arr[0]
        return d


def int_to_txt(inp_path, out_path, wst_dict, unknown_symbol=None):
    ''' based on:  cat exp/tri2a/decode/scoring/15.tra | utils/int2sym.pl -f 2-
    exp/tri2a/graph/words.txt | sed s:\<UNK\>::g'''
    if unknown_symbol is None:
        unknown_symbol = '\<UNK\>'
    with open(inp_path, 'r') as r:
        with open(out_path, 'w') as w:
            for line in r:
                tmp = line.split()
                name_align, dec = tmp[0], tmp[1:]
                # For now we are throwing away align
                name = name_align.split('_')[0]
                w.write('%s ' % name)
                for iw in dec:
                    try:
                        word = wst_dict[iw]
                    except KeyError:
                        print 'Warning: unknown word %s' % iw
                        word = unknown_symbol
                    w.write('%s ' % word)
                w.write('\n')


def compact_hyp(hyp_path, comp_hyp_path):
    d = DefaultOrderedDict(list)
    with open(hyp_path, 'rb') as hyp:
        for line in hyp:
            tmp = line.split()
            name, dec = tmp[0], tmp[1:]
            d[name].extend(dec)
    with open(comp_hyp_path, 'wb') as w:
        for wav, dec_list in d.iteritems():
            w.write('%s %s\n' % (wav, ' '.join(dec_list)))
