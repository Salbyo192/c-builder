#!/bin/zsh

# a script to push changes to 'main'.

if [[ $# -eq 0 ]]; then
  echo "usage: $0 <commit message>"
  exit 1
fi

git add .
git commit -m $1
git push
