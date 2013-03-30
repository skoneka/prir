# -*- coding: utf-8 -*-
# python <3
# 2013 Artur Skonecki, Łukasz Załęski
# install extra packages:
# pip install apachelog mpi4py
import apachelog, sys
from mpi4py import MPI
from operator import itemgetter
from itertools import groupby
from collections import defaultdict

# grouper '%h' | '%t'
def MR_map(data, grouper): 
  divided_data = None
  if data:
    basic_range = len(data) / size
    extended_range = len(data) % size
    divided_data = [data[i*basic_range:(i+1)*basic_range] for i in range(size)]
    extended_data = data[size * basic_range:size * basic_range + extended_range]
    divided_data[0] += extended_data
  divided_data = comm.scatter(divided_data, root=0)
  #print('rank '+ str(rank) +' len divided_data '+str(len(divided_data)))
  divided_data = sorted(divided_data, key=itemgetter(grouper))
  groups = defaultdict(list)
  for key, group in groupby(divided_data, itemgetter(grouper)):
    for thing in group:
      groups[key].append( thing )
  data = comm.gather(groups, root=0)
  if rank != 0:
    assert data is None
  return data

# merge all entries from ranked data dicts
def MR_reduce(ranked_data):
  dd = defaultdict(list)
  for d in ranked_data:
    for key, value in d.iteritems():
      dd[key] += value
  return dd

def main():
  if rank == 0:
    from optparse import OptionParser
    
    parser = OptionParser(usage = 'Usage: apachelogstats.py --(addr|time) /var/log/apache2/access.log.1')
    
    parser.add_option("-t", "--time",
    action="store_const", dest="mode", const = '%t',
    help="group by time")
    parser.add_option("-a", "--addr",
    action="store_const", dest="mode", const = '%h',
    help="group by address")
    
    (options, args) = parser.parse_args()
    
    # Format copied and pasted from Apache conf - use raw string + single quotes
    format = r'%h %l %u %t \"%r\" %>s %b \"%{Referer}i\" \"%{User-Agent}i\"'

    p = apachelog.parser(format)
    data = []

    for i,line in enumerate(open(args[0])):
      try:
        data.append( p.parse( line ) )
      except:
        sys.stderr.write("Unable to parse %s" % line)
      if i > 6: break
  else:
    data = None
    options = None

  options = comm.bcast(options, root=0)
  
  ranked_data = MR_map(data, options.mode)
  
  if rank == 0:
    dd = MR_reduce(ranked_data)
    
    for key, value in dd.iteritems():
      print( '%s %d' % ( str(key), len(value) ) )

if __name__ == '__main__':
  comm = MPI.COMM_WORLD
  size = comm.Get_size()
  rank = comm.Get_rank()
  main()



