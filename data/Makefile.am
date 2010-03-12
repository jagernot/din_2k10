## Process this file with automake to produce Makefile.in

dindatadir = $(localstatedir)/@PACKAGE@

dindata_DATA = \
fm.txt\
am.txt\
patl.txt\
patr.txt\
strength_editor.txt\
strength_editor_help.txt\
wave_editor.txt\
channel_editor.txt\
wav_lib.txt\
drone.txt\
drone_editor.txt\
din_help.txt\
gater_editor.txt\
gater_pattern_lib.txt\
mod_editor.txt\
jag.fnt\
default_map.txt\
default_wave.txt\
default_left.txt\
default_right.txt\
din_info\
one\
resonators\
globals\
ji.txt\
font_editor.txt\
ragalist\
sine_levels\
fft_levels\
et.txt\
bhairav.drone\
blues.drone\
bhoopali.drone\
darbari.drone\
jog.drone\
malkauns.drone\
multani.drone\
shivaranjani.drone\
todi.drone\
yaman.drone\
m00\
coml.txt\
comr.txt\
compressor_editor.txt\
curve_editor.hlp\
pytha.txt

dinbindir = ${prefix}/bin
dinbin_DATA = checkdotdin

EXTRA_DIST = $(dindata_DATA) $(dinbin_DATA)

install-data-hook: ${DESTDIR}/$(prefix)/bin/checkdotdin
	chmod +x $^
