#!/bin/bash
repo_root="$(git rev-parse --show-toplevel)"

echo "Note that the context is much bigger than from dockerhub, if you build it locally."
docker build --pull -f $repo_root/docker/production.dockerfile -t ufaldsg/pykaldi $repo_root
# # For debugging remove the --rm option
# docker build --pull --rm -f $repo_root/docker/production.dockerfile -t ufaldsg/pykaldi $repo_root
