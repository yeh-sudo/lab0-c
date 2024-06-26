# Test performance of listsort with random and descending orders
# 10000: all correct sorting algorithms are expected pass
# 50000: sorting algorithms with O(n^2) time complexity are expected failed
# 100000: sorting algorithms with O(nlogn) time complexity are expected pass
option fail 0
option malloc 0
new
ih RAND 10000
listsort
reverse
listsort
free
new
ih RAND 50000
listsort
reverse
listsort
free
new
ih RAND 100000
listsort
reverse
listsort
free
option descend 1
new
ih RAND 10000
listsort
reverse
listsort
free
new
ih RAND 50000
listsort
reverse
listsort
free
new
ih RAND 100000
listsort
reverse
listsort
free
