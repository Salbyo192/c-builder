#!/bin/zsh

# a script to push releases to a repository.

if [ "$#" -eq 0 ]; then
    echo "usage: $0 <tag> <file> <title> <notes>"
    exit 1
fi

gh release create $1 $2 --title $3 --notes $4 --latest
