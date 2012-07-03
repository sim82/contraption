VPP_MAJOR=7
VPP_MINOR=2

EPAV_MAJOR=4
EPAV_MINOR=3

if [ -d contraption ]
then
echo contraption dir exists
exit 1
fi

# get latest release version (currently it's just the master branch). Could be tagged commit etc...

git clone git@github.com:sim82/contraption.git
cd contraption

git submodule init
git submodule update

cat > build_versions.h << EOF
// created by deploy.sh

namespace versions {
    namespace visual_papara {
        const int major = $VPP_MAJOR;
        const int minor = $VPP_MINOR;
    }
    
    namespace epa_viewer {
        const int major = $EPAV_MAJOR;
        const int minor = $EPAV_MINOR;
    }
}
EOF

# remove git metadata
rm -rf `find . -type d -name .git`
rm .gitmodules
cd ..


DEP_DIR=visual_papara-$VPP_MAJOR.$VPP_MINOR
DEP_NAME=$DEP_DIR.tar.gz

if [ -d $DEP_DIR ]
then
rm -rf $DEP_DIR
fi

mv contraption $DEP_DIR
tar czf $DEP_NAME $DEP_DIR