# follow https://google.github.io/tcmalloc/tuning.html#system-level-optimizations
sudo -S bash -c "echo always > /sys/kernel/mm/transparent_hugepage/enabled"
sudo -S bash -c "echo defer+madvise > /sys/kernel/mm/transparent_hugepage/defrag"
sudo -S bash -c "echo 0 > /sys/kernel/mm/transparent_hugepage/khugepaged/max_ptes_none"
sudo -S bash -c "echo 1 > /proc/sys/vm/overcommit_memory"