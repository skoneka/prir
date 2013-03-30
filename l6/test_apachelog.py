# -*- coding: utf-8 -*-
# Artur Skonecki, Łukasz Załęski
import apachelog, sys
from mpi4py import MPI
from operator import itemgetter
from itertools import groupby
from collections import defaultdict

comm = MPI.COMM_WORLD
size = comm.Get_size()
rank = comm.Get_rank()


if rank == 0:
  # Format copied and pasted from Apache conf - use raw string + single quotes
  format = r'%h %l %u %t \"%r\" %>s %b \"%{Referer}i\" \"%{User-Agent}i\"'

  p = apachelog.parser(format)
  data = []

  for i,line in enumerate(open('/var/log/apache2/access.log.1')):
    try:
      data.append( p.parse( line ) )
    except:
      sys.stderr.write("Unable to parse %s" % line)
    if i > 6: break
else:
  data = None


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
  print('rank '+ str(rank) +' len divided_data '+str(len(divided_data)))
  #print('rank '+ str(rank) +' data '+str(data[0]))
  divided_data = sorted(divided_data, key=itemgetter(grouper))
  groups = defaultdict(list)
  for key, group in groupby(divided_data, itemgetter(grouper)):
    for thing in group:
      groups[key].append( thing )
      print ('rank %d at %s append ' % (rank, str(key)))
      #print "A %s is a %s." % (thing['%r'], key)
  #print( groups )
  data = comm.gather(groups, root=0)
  if rank == 0:
    for item in data:
      print len(item)
  else:
    assert data is None
  return data
    
ranked_data = MR_map(data, '%t')

# merge all entries from ranked data dicts
def MR_reduce(ranked_data):
  dd = defaultdict(list)
  for d in ranked_data:
    for key, value in d.iteritems():
      dd[key] += value
  return dd

if rank == 0:
  dd = MR_reduce(ranked_data)
  
  #print dd
  for key, value in dd.iteritems():
    print( '%s %d' % ( str(key), len(value) ) )



