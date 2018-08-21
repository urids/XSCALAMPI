#!/bin/bash
git add -f --all * && git commit -m "composition $(date +%Y%m%d) before task_t API changes" && git push origin alpha01
