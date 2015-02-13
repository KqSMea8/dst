#!/bin/sh

mkdir -p output/
mkdir -p output/lib
mkdir -p output/lib/tool/
cp ../bios-check/bioscfg.py output/
cp gparas.py output/lib/
cp __init__.py output/lib/
cp machinfo.py output/lib/
cp getmachineinfo.py output/
cp logtrace.py output/lib/
cp parserule.py output/lib/
cp cpumeminfo.py output/lib/
cp main output/

cp tool/pci.ids output/lib/tool/
cp tool/dmidecode output/lib/tool/
cp tool/smartctl output/lib/tool/
cp tool/MegaCli output/lib/tool/
cp tool/ssd_label output/lib/tool/
cp tool/ssd_badblock output/lib/tool/
cp tool/ssd_ecinfo output/lib/tool/
cp tool/dimmpresent output/lib/tool/
cp tool/readtemperature output/lib/tool/

cp tool/hpacucli.tar.gz output/lib/tool/
cp tool/lsi.tar.gz output/lib/tool/
