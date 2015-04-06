#!/bin/bash
repo_root="$(git rev-parse --show-toplevel)"

docker build --rm -f $repo_root/docker/development/Dockerfile -t ufaldsg/pykaldi-dev $repo_root
