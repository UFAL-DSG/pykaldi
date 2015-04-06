#!/bin/bash
repo_root="$(git rev-parse --show-toplevel)"

echo "Note that the context is much bigger than from dockerhub, if you build it locally."
# For debugging remove the --rm option
docker build --pull --rm -f $repo_root/docker/production/Dockerfile -t ufaldsg/pykaldi $repo_root
