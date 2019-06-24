#!/bin/bash

# This script will initialzie the plugin template for development,
# and set up a version control system using git.

if [ "$#" -ne 1 ]; then
    echo "Please enter one command line arg for the name of the plugin"
    exit
fi

echo "------------------------------------"
echo "Updating ADPluginTemplate with name:"
echo $1

python3 update_name.py -n $1

cd ..

echo "" >> .gitignore
echo "scripts/" >> .gitignore
echo "TemplateReadme.md" >> .gitignore

mv README.md TemplateReadme.md

PluginName="${1,,}"
PluginName="${PluginName^}"

touch README.md
echo "# ADPlugin$PluginName" >> README.md

git init
git add -A

git commit -m "Initial Commit for ADPlugin$PluginName"

