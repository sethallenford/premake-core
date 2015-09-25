#!/bin/sh
set -e

SCRIPT_DIR=$( cd ${0%/*} && pwd -P )
cd "${SCRIPT_DIR}/../.."

echo "Removing old premake-public"
rm -rf public
mkdir public
cd public

echo "Cloning new clean copies"
git clone git@ghosthub.corp.blizzard.net:premake/module-packagemanager.git --branch master --single-branch premake-packagemanager
git clone git@ghosthub.corp.blizzard.net:premake/module-blizzard.git       --branch master --single-branch premake-blizzard
git clone git@ghosthub.corp.blizzard.net:premake/module-consoles.git       --branch master --single-branch premake-consoles
git clone git@ghosthub.corp.blizzard.net:premake/module-xcode.git          --branch master --single-branch premake-xcode
git clone git@ghosthub.corp.blizzard.net:premake/module-android.git        --branch master --single-branch premake-android
git clone git@ghosthub.corp.blizzard.net:premake/premake-core.git          --branch master --single-branch premake-core

echo "Scrubbing repositories"
java -jar ${SCRIPT_DIR}/bfg.jar --replace-text ${SCRIPT_DIR}/replacements.txt --delete-folders publish --no-blob-protection ./premake-packagemanager
java -jar ${SCRIPT_DIR}/bfg.jar --replace-text ${SCRIPT_DIR}/replacements.txt --delete-folders publish --no-blob-protection ./premake-blizzard
java -jar ${SCRIPT_DIR}/bfg.jar --replace-text ${SCRIPT_DIR}/replacements.txt --delete-folders publish --no-blob-protection ./premake-consoles
java -jar ${SCRIPT_DIR}/bfg.jar --replace-text ${SCRIPT_DIR}/replacements.txt --delete-folders publish --no-blob-protection ./premake-xcode
java -jar ${SCRIPT_DIR}/bfg.jar --replace-text ${SCRIPT_DIR}/replacements.txt --delete-folders publish --no-blob-protection ./premake-android
java -jar ${SCRIPT_DIR}/bfg.jar --replace-text ${SCRIPT_DIR}/replacements.txt --delete-folders publish --no-blob-protection ./premake-core

echo "Pushing repositories"

cd premake-packagemanager
git reset --hard
git reflog expire --expire=now --all
git gc --prune=now --aggressive
git remote set-url origin https://github.com/blizzard/premake-packagemanager.git
git push origin master -f

cd ../premake-blizzard
git reset --hard
git reflog expire --expire=now --all
git gc --prune=now --aggressive
git remote set-url origin https://github.com/blizzard/premake-blizzard.git
git push origin master -f

cd ../premake-consoles
git reset --hard
git reflog expire --expire=now --all
git gc --prune=now --aggressive
git remote set-url origin https://github.com/blizzard/premake-consoles.git
git push origin master -f

cd ../premake-xcode
git reset --hard
git reflog expire --expire=now --all
git gc --prune=now --aggressive
git remote set-url origin https://github.com/blizzard/premake-xcode.git
git push origin master -f

cd ../premake-android
git reset --hard
git reflog expire --expire=now --all
git gc --prune=now --aggressive
git remote set-url origin https://github.com/blizzard/premake-android.git
git push origin master -f

cd ../premake-core
git reset --hard
git reflog expire --expire=now --all
git gc --prune=now --aggressive

git submodule init
git submodule update --remote
git add .
git commit -m fixup-submodule-references

git remote set-url origin https://github.com/blizzard/premake-core.git
git push origin master -f

cd ..
