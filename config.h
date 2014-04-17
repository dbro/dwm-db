/* See LICENSE file for copyright and license details. */
#include "bstackhoriz.c"

/* appearance */
static const char font[]            = "inconsolata:size=15";
#define NUMCOLORS 7                     // need at least 3
static const char colors[NUMCOLORS][ColLast][8] = {
   // border   foreground  background
   { "#cccccc", "#888888", "#000000" }, // 0 = normal
   { "#cc0000", "#ffff00", "#000000" }, // 1 = selected
   { "#ff6600", "#000000", "#ff6600" }, // 2 = urgent/warning
   { "#000000", "#ffffff", "#000000" }, // 3 = window title text
   { "#000000", "#ffffff", "#000000" }, // 4 = statusbar elevated level
   { "#000000", "#ff0000", "#000000" }, // 5 = statusbar critical level
   { "#ff6600", "#000000", "#ff0000" }, // 6 = statusbar critical level flashing
};
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const Bool showbar           = True;     /* False means no bar */
static const Bool topbar            = True;     /* False means bottom bar */

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            True,        -1 },
    /* { "Firefox",  NULL,       NULL,       1 << 8,       False,       -1 }, */
    { "Skype",    NULL,       NULL,       0,            True,        -1 },
    { "Vlc",      NULL,       NULL,       0,            True,        -1 },
};

/* layout(s) */
static const float mfact      = 0.5; /* factor of master area size [0.05..0.95] */
static const Bool resizehints = True; /* True means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "===",      bstackhoriz },
};

/* key definitions */
#define MODKEY Mod4Mask // use windows key
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static const char *dmenucmd[] = { "dmenu_run", "-fn", font, "-nb", colors[0][ColBG], "-nf", colors[0][ColFG], "-sb", colors[1][ColBG], "-sf", colors[1][ColFG], NULL };
static const char *termcmd[]  = { "st", NULL };
static const char *lockscreencmd[]  = { "slock", NULL };
static const char *browsercmd[]     = { "google-chrome", NULL };
static const char *browseraltcmd[]  = { "firefox", NULL };
static const char *filemgrcmd[]     = { "thunar", NULL };
static const char *netmgrcmd[]      = { "xterm", "-e", "wicd-curses", NULL };
static const char *screenshotcmd[]  = { "scrot", NULL };
/* static const char *editorcmd[]      = { "vim", NULL }; */
static const char *musicplay[]      = { "muss", "-p", NULL };
static const char *musicstop[]      = { "muss", "-s", NULL };
static const char *musicnext[]      = { "muss", "-n", NULL };
static const char *musicback[]      = { "muss", "-b", NULL };
static const char *musickcrw[]      = { "muss", "kcrw-live", NULL };
static const char *musickcrwalt[]   = { "muss", "kcrw-music", NULL };
static const char *volumeup[]       = { "amixer", "-q", "sset", "Master", "5+", NULL };
static const char *volumedown[]     = { "amixer", "-q", "sset", "Master", "5-", NULL };
static const char *volumemute[]     = { "amixer", "-q", "sset", "Master", "toggle", NULL };

#include "movestack.c"
static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
        { MODKEY,                       XK_q,      spawn,          {.v = lockscreencmd } },
        { MODKEY,                       XK_b,      spawn,          {.v = browsercmd } },
        { MODKEY|ShiftMask,             XK_b,      spawn,          {.v = browseraltcmd } },
        { MODKEY,                       XK_e,      spawn,          {.v = filemgrcmd } },
        { MODKEY|ShiftMask,             XK_n,      spawn,          {.v = netmgrcmd } },
        { 0,                            XK_Print,  spawn,          {.v = screenshotcmd } },
        { MODKEY,                       XK_Up,     spawn,          {.v = musicplay } },
        { MODKEY,                       XK_Down,   spawn,          {.v = musicstop } },
        { MODKEY,                       XK_Right,  spawn,          {.v = musicnext } },
        { MODKEY,                       XK_Left,   spawn,          {.v = musicback } },
        { MODKEY,                       XK_s,      spawn,          {.v = musickcrw } },
        { MODKEY|ShiftMask,             XK_s,      spawn,          {.v = musickcrwalt } },
        { 0,                            0x1008ff13,spawn,          {.v = volumeup } },
        { 0,                            0x1008ff11,spawn,          {.v = volumedown } },
        { 0,                            0x1008ff12,spawn,          {.v = volumemute } },
        { MODKEY,                       XK_u,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_j,      movestack,      {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,      movestack,      {.i = -1 } },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY|ShiftMask,             XK_t,      setlayout,      {.v = &layouts[3]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

