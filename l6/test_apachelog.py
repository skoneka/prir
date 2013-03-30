import apachelog, sys
from mpi4py import MPI

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



def MR_map(data): 
  divided_data = None
  if data:
    basic_range = len(data) / size
    extended_range = len(data) % size
    print(len(data))
    print(basic_range)
    print(extended_range)
    divided_data = [data[i*basic_range:(i+1)*basic_range] for i in range(size)]
    extended_data = data[size * basic_range:size * basic_range + extended_range]
    fromto =  ['%d:%d' %( i*basic_range, (i+1)*basic_range ) for i in range(size) ]
    divided_data[0] += extended_data
    #import ipdb
    #ipdb.set_trace()
    #divided_data[0] +=  data(
  divided_data = comm.scatter(divided_data, root=0)
  print('rank '+ str(rank) +' len divided_data '+str(len(divided_data)))
  #print('rank '+ str(rank) +' data '+str(data[0]))

MR_map(data)

