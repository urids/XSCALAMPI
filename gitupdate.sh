#!/bin/bash
git add -f * && git commit -m "composition $(date +%Y%m%d) disaster recovery" && git push origin master
