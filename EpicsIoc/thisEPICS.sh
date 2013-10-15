#
#  This sets EPICS variables
#
EPICS_TOP=/opt/epics

export EPICS_BASE=$EPICS_TOP/base
export EPICS_HOST_ARCH=$($EPICS_TOP/base/startup/EpicsHostArch)
export EPICS_EXTENSIONS=/opt/epics/extensions

export PATH=$PATH:$EPICS_BASE/bin/$EPICS_HOST_ARCH:$EPICS_EXTENSTIONS/bin/$EPICS_HOST_ARCH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/$EPICS_BASE/lib/$EPICS_HOST_ARCH
