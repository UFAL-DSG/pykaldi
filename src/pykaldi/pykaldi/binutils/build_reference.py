#!/usr/bin/env python
# encoding: utf-8

import glob
import sys
import os


def build_reference(wav_scp, ref_path):
    with open(ref_path, 'w') as w:
        with open(wav_scp, 'r') as scp:
            for line in scp:
                name, wavpath = line.strip().split(' ', 1)
                with open(wavpath + '.trn') as trn:
                    trans = trn.read().strip()
                    w.write('%s %s\n' % (name, trans))


if __name__ == '__main__':
    usage = '''
    Usage: python %(exec)s (audio_directory|in.scp) decode_directory

    Where directory contains files "*.scp" and
    audio files "*.wav" and their transcriptions "*.wav.trn".
    The "*.scp" files contains of list wav names and their path.

    The %(exec)s looks for "*.scp" files builds a reference from "*.wav.trn"
    '''
    usage_args = {'exec': sys.argv[0]}

    if len(sys.argv) != 3:
        print >> sys.stderr, "Wrong number of arguments"
        print >> sys.stderr, usage % {'exec': sys.argv[0]}
        sys.exit(1)

    if sys.argv[1].endswith('scp'):
        scps = [sys.argv[1]]
    else:
        scps = glob.glob(os.path.join(sys.argv[1], '*.scp'))
    target_dir = sys.argv[2]
    if not len(scps):
        print >> sys.stderr, "No '*.scp' files found"
        print >> sys.stderr, usage % {'exec': sys.argv[0]}
        sys.exit(1)
    if not os.path.isdir(target_dir):
        print >> sys.stderr, "No '*.scp' files found"
        print >> sys.stderr, usage % {'exec': sys.argv[0]}
        sys.exit(1)

    refers = [os.path.join(target_dir, os.path.basename(scp) + '.tra') for scp in scps]
    for scp, refer in zip(scps, refers):
        build_reference(scp, refer)
