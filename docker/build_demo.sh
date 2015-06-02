#!/bin/bash
repo_root="$(git rev-parse --show-toplevel)"

docker build --rm -f $repo_root/docker/demo/Dockerfile -t ufaldsg/pykaldi-demo $repo_root
