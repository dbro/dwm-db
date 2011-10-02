#!/bin/sh

# statusbar for dwm window manager
# displays various status info
# only the important stuff,
# as dense and readable as possible

# depends on mpc, xsetroot
# and dwm's statuscolors patch. we use these codes:
COL_NORM=`echo "\001"`
COL_WARN=`echo "\005"`
COL_CRIT=`echo "\006"`
COL_CRIT_ALT=`echo "\007"`

# TODO: a few things should be made into variables
# eg. eth0 wlan0 battery0

SEP=" | "

# a simple flag to enable flashing effects
FLIPFLOP=0

# gather initial set of information for rate calculations
VMSTAT=`vmstat -s`
NETDEV=`cat /proc/net/dev`
TICKS_PREV=`date +%s%N | cut -b 1-12`
CPUIDLETICKS_PREV=`echo "$VMSTAT" | sed -n -e 's/^ *\([0-9]*\).*idle cpu.*$/\1/p'`
NETUP_PREV=`echo "$NETDEV" | awk -F"[ :]*" '/eth0/ {print $11}'`
NETDN_PREV=`echo "$NETDEV" | awk -F"[ :]*" '/eth0/ {print $3}'`
if [ `cat /proc/net/wireless | grep wlan0 | wc -l` -gt 0 ]
then
  WLANUP=`echo "$NETDEV" | awk -F"[ :]*" '/wlan0/ {print $11}'`
  WLANDN=`echo "$NETDEV" | awk -F"[ :]*" '/wlan0/ {print $3}'`
  NETUP_PREV=$(( $NETUP_PREV + $WLANUP ))
  NETDN_PREV=$(( $NETDN_PREV + $WLANDN ))
  WLAN_FOUND="yes";
fi

while true
do
  sleep 2
  VMSTAT=`vmstat -s`
  NETDEV=`cat /proc/net/dev`
  TICKS_NOW=`date +%s%N | cut -b 1-12`

  SWAP_USED=`echo "$VMSTAT" | sed -n -e 's/^ *\([0-9]*\).*used swap.*$/\1/p'`
  SWAP_TOTAL=`echo "$VMSTAT" | sed -n -e 's/^ *\([0-9]*\).*total swap.*$/\1/p'`
  SWAP_PCT=$(( 100 * $SWAP_USED / $SWAP_TOTAL ))
  MEM_USED=`echo "$VMSTAT" | sed -n -e 's/^ *\([0-9]*\).*used memory.*$/\1/p'`
  MEM_TOTAL=`echo "$VMSTAT" | sed -n -e 's/^ *\([0-9]*\).*total memory.*$/\1/p'`
  MEM_PCT=$(( 100 * $MEM_USED / $MEM_TOTAL ))

  CPUIDLETICKS=`echo "$VMSTAT" | sed -n -e 's/^ *\([0-9]*\).*idle cpu.*$/\1/p'`
  CPU_IDLE_PCT=$(( 100 * ($CPUIDLETICKS - $CPUIDLETICKS_PREV) / ($TICKS_NOW - $TICKS_PREV) ))
  CPU_PCT=$(( 100 - $CPU_IDLE_PCT ))

  NETUP=`echo "$NETDEV" | awk -F"[ :]*" '/eth0/ {print $11}'`
  NETDN=`echo "$NETDEV" | awk -F"[ :]*" '/eth0/ {print $3}'`
#  echo "1 NETUP = $NETUP \t\tNETDN = $NETDN"
  if [ -n "$WLAN_FOUND" ]
  then
    WLANUP=`echo "$NETDEV" | awk -F"[ :]*" '/wlan0/ {print $11}'`
    WLANDN=`echo "$NETDEV" | awk -F"[ :]*" '/wlan0/ {print $3}'`
#    echo "1 WLANUP = $WLANUP \t\tWLANDN = $WLANDN"
    NETUP=$(( $NETUP + $WLANUP ))
    NETDN=$(( $NETDN + $WLANDN ))
#    echo "2 NETUP = $NETUP \t\tNETDN = $NETDN"
    WLAN_PCT=`cat /proc/net/wireless | sed -n -e 's/.*wlan0[^0-9]*[0-9]* *\([0-9]*\).*/\1/p'`
  fi
  NETUP_RATE=$(( ($NETUP - $NETUP_PREV) * 100 / ($TICKS_NOW - $TICKS_PREV)  ))
  NETDN_RATE=$(( ($NETDN - $NETDN_PREV) * 100 / ($TICKS_NOW - $TICKS_PREV)  ))

  if which acpi > /dev/null
  then
    ACPI=`acpi -b 2>&1 | head -n 1`
#    if [ `echo "$ACPI" | cut -d' ' -f1,2` != "No support" ]
    ACPI2=`echo "$ACPI" | cut -d' ' -f1,2`
    if [ "$ACPI2" != "No support" ]
    then
      BATT_PCT=`echo "$ACPI" | awk -F"[ ,%]*" \
        '/att/ { print prefix $4 }'`
      BATT_CHARGING=`echo "$ACPI" | awk -F"[ ,%]*" \
        '/att/ {
          if (tolower($0) ~ / charging/) print "+"; else print " " }'`
    else
      BATT_PCT=""
    fi
  else
    BATT_PCT=""
  fi

  MUSIC=`mpc --format "[[%artist% - ]%title%]|[%name%]|[%file%]"`
  if [ 1 -eq `echo "$MUSIC" | wc -l` ] 
  then
    MUSIC=""
  else
    MUSIC=`echo "$MUSIC" | head -n 1 | head -c 30`
  fi

  DATE=`date +"%a %b %-d %H:%M"`

  # Assemble and format the information
  if [ $CPU_PCT -ge 99 ] 
  then
    CPUDIGIT="X"
  else
    CPUDIGIT=$(( $CPU_PCT / 10 ))
  fi
  if   [ $CPU_PCT -ge 90 ]; then COLON="$COL_CRIT"; COLOFF="$COL_NORM"
  elif [ $CPU_PCT -ge 20 ]; then COLON="$COL_WARN"; COLOFF="$COL_NORM"
  else COLON=""; COLOFF=""; fi
  CPU="${COLON}${CPUDIGIT}c${COLOFF}"

  if [ $MEM_PCT -ge 99 ] 
  then
    MEMDIGIT="X"
  else
    MEMDIGIT=$(( $MEM_PCT / 10 ))
  fi
  if   [ $MEM_PCT -ge 80 ]; then COLON="$COL_CRIT"; COLOFF="$COL_NORM"
  elif [ $MEM_PCT -ge 50 ]; then COLON="$COL_WARN"; COLOFF="$COL_NORM"
  else COLON=""; COLOFF=""; fi
  MEM="${COLON}${MEMDIGIT}m${COLOFF}"

  if [ $SWAP_PCT -ge 99 ] 
  then
    SWAPDIGIT="X"
  else
    SWAPDIGIT=$(( $SWAP_PCT / 10 ))
  fi
  if   [ $SWAP_PCT -ge 80 ]; then COLON="$COL_CRIT"; COLOFF="$COL_NORM"
  elif [ $SWAP_PCT -ge 10 ]; then COLON="$COL_WARN"; COLOFF="$COL_NORM"
  else COLON=""; COLOFF=""; fi
  SWAP="${COLON}${SWAPDIGIT}s${COLOFF}"

  if [ -z "$WLAN_FOUND" ] || [ $WLAN_PCT -eq 0 ]
  then
    WLAN=""
  else
    if [ $WLAN_PCT -le 20 ]; then COLON="$COL_CRIT"; COLOFF="$COL_NORM"
    elif [ $WLAN_PCT -le 30 ]; then COLON="$COL_WARN"; COLOFF="$COL_NORM"
    else COLON=""; COLOFF=""; fi
    WLAN=${COLON}$(( $WLAN_PCT / 10 ))"w$COLOFF "
  fi

  NET=`printf '%3dn%-3d' \
    $(( $NETDN_RATE / 1024 )) \
    $(( $NETUP_RATE / 1024 ))`

  if [ -n "$BATT_PCT" ]
  then
    if [ $BATT_PCT -ge 99 ] 
    then
      BATTDIGIT="**"
    else
      # breaking the separation between information and presentation,
      # the charging variable holds the visual representation
      BATTDIGIT=${BATT_CHARGING}$(( $BATT_PCT / 10 ))
    fi
    if [ $BATT_PCT -le 20 ]; then
        if [ "$BATT_CHARGING" = " " -a $FLIPFLOP -eq 0 ]; then
            COLON="$COL_CRIT_ALT"
            FLIPFLOP=1
        else COLON="$COL_CRIT"; FLIPFLOP=0; fi
        COLOFF="$COL_NORM"
    elif [ $BATT_PCT -le 50 ]; then COLON="$COL_WARN"; COLOFF="$COL_NORM"
    else COLON=""; COLOFF=""; fi
    BATT=${SEP}${COLON}${BATTDIGIT}"b"${COLOFF}
  else
    BATT=""
  fi
 
  xsetroot -name "$MUSIC$SEP$CPU $MEM $SWAP $SEP$WLAN$NET$BATT$SEP$DATE"

  # Store the current values to prepare for next loop's rate calculations
  TICKS_PREV=$TICKS_NOW
  CPUIDLETICKS_PREV=$CPUIDLETICKS
  NETUP_PREV=$NETUP
  NETDN_PREV=$NETDN
done
