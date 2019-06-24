# ADPluginTemplate

This repository contains a pre-organized file structure for writing areaDetector plugins.
To use it, start by downloading a zip of one of the releases.

**DO NOT CLONE THIS REPOSITORY WHEN WRITING A NEW PLUGIN**

If you do, the git information will be cloned as well, and version control for your plugin will
contain all changes made to this template.

Once you download and unzip the template, navigate to the scripts directory as follows:
```
cd scripts
```
inside, you will find two scripts:

Script | Description | Usage
--------|------------------------|--------------
update_name.py | A python script that updates occurances of the plugin name | python3 update_name.py -n $NAME
initializePlugin.sh | A bash script that runs the python script and inits git version control | ./initializePlugin.sh $NAME

To fully automate the initial setup, you may run the `initializePlugin.sh` script (requires bash > 4.0). This will run the `update_name.py` script, and initialize a git repo, and make an initial commit. You will need to add a remote repository in order to be able to push the git changes however, and a global git config is required for the git commands to function correctly.

Alternatively, you may run the `update_name.py` script by itself, which will update all occurances of the plugin name in the template to a name of your choosing.

Also, make sure to replace this README.md file with one that explains how to use your new plugin!
