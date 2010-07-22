from bisect import bisect_right, bisect_left
IN_SHADOW = object()

def get_interval(intervals, x):
    """Return a the interval that contains x.
    """
    starts = [s for s,_ in intervals]
    index = bisect_right(starts, x) - 1
    if x < intervals[index][1]:
        return index
    elif x == intervals[index][1]:
        return index+1
    
def overlapping(intervals, val):
    """Return a subset of intervals that overlap val.

    Uses binary search so lookup should be log(n).
    """
    starts = [s for s,_ in intervals]
    (a,b) = val
    start_index = bisect_right(starts, a) - 1
    end_index = bisect_left(starts, b) - 1
    return max(0,start_index), end_index

class ImmutableList(object):
    __slots__ = ["base", "shadow", "intervals", "offsets", "size"]

    def __init__(self, base):
        self.base = base
        self.size = len(self.base) # the effective size (as seen from outside)
        self.shadow = {}           # for keeping inserted items
        self.intervals = [(0,self.size)]
        self.offsets = [0]

    def __getitem__(self, i):
        if isinstance(i, slice):
            start, stop, step = i.start, i.stop, i.step
            if start is None: start = 0
            if stop is None: stop = len(self)
            if step is None: step = 1
            return [self[j] for j in range(start, stop, step)]

        if not 0 <= i < self.size:
            raise IndexError(i)

        offset = self.offsets[get_interval(self.intervals,i)]
        if offset is not IN_SHADOW:
            return self.base[i+offset]
        else:
            return self.shadow[i]
    
    def remove(self, a, b):
        assert b > a

        start, end = overlapping(self.intervals, (a,b))

        # Split overlapping intervals up
        se,te = self.intervals[end]
        if te != b:
            self.intervals[end:end+1] = [(se,b), (b,te)]
            self.offsets[end:end+1] = [self.offsets[end]]*2

        ss,ts = self.intervals[start]
        if ss != a:
            self.intervals[start:start+1] = [(ss,a), (a,ts)]
            self.offsets[start:start+1] = [self.offsets[start]]*2

        # Figure out what range of intervals we want to delete
        if ss != a:         
            if te != b:         #   [   ]
                ds = start+1    # [][][ ][]
                de = end+1
            else:               #   [     ]
                ds = start+1    # [][][   ]
                de = end+1
        else:
            if te != b:         # [     ]
                ds = start      # [  ][ ][]
                de = end
            else:               # [       ]
                ds = start      # [  ][   ]
                de = end

        # Clear them out of the shadow dict if necessary
        for i in range(ds, de+1):
            if self.offsets[i] is IN_SHADOW:
                for j in range(*self.intervals[i]):
                    del self.shadow[j]

        # Delete the intervals
        del self.intervals[ds:de+1]
        del self.offsets[ds:de+1]

        # Update the size
        sz = b - a
        self.size -= sz

        # Shift everything else down
        self._apply_shift(ds, sz)

    def insert(self, a, seq):
        sz = len(seq)
        self.size += sz

        if not self.intervals:
            self.intervals.insert(0, (a,a+sz) )
            self.offsets.insert(0, IN_SHADOW)
            self.shadow.update(((i,v) for i,v in zip(range(a, a+sz), seq)))
            return

        s = get_interval(self.intervals, a)

        if s != len(self.intervals):
            ss,ts = self.intervals[s]

            if ss != a:
                self.intervals[s:s+1] = [(ss,a), (a,ts)]
                self.offsets[s:s+1] = [self.offsets[s]]*2
            
            if ss != a:         
                mod_start = s+1
            else:
                mod_start = s
            
            self._apply_shift(mod_start,-sz)
        else:
            mod_start = s

        self.intervals.insert(mod_start, (a,a+sz) )
        self.offsets.insert(mod_start, IN_SHADOW)
        self.shadow.update(((i,v) for i,v in zip(range(a, a+sz), seq)))

    def _apply_shift(self, ds, sz):
        shadow_updates = []
        for i in range(ds,len(self.intervals)):
            s,t = self.intervals[i]

            if self.offsets[i] is not IN_SHADOW:
                # It's in the original array, just apply an offset
                self.offsets[i] += sz
            else:
                # It's in the shadow dict, take out that range and shift it
                vals = [(x,self.shadow[x]) for x in range(s,t)]
                for x in range(s,t):
                    del self.shadow[x]
                shadow_updates += [(j-sz,v) for j,v in vals]

            self.intervals[i] = (s-sz, t-sz)

        self.shadow.update(shadow_updates)
        
    def __delslice__(self, i, j):
        self.remove(i,j)        

    def __setslice__(self, i, j, seq):
        self.remove(i,j)
        self.insert(i,seq)

    def __setitem__(self, i, x):
        self[i:i+1] = [x]

    def __len__(self):
        return self.size

    def optimize(self):
        i = len(self.intervals) - 1
        while i > 0:
            i -= 1
            if self.offsets[i] == self.offsets[i+1]:
                a,b = self.intervals[i][0],self.intervals[i+1][1]
                self.intervals[i:i+2] = [(a,b)]
                self.offsets[i:i+2] = [self.offsets[i]]

    def single_interval(self, ival):
        a,b = ival
        return ' '*ival[0] + '[' + ' '*(b-a-2) + ']'

    def interval_string(self):
        s = ''
        for (i,j),o in zip(self.intervals,self.offsets):
            if o is IN_SHADOW:
                char = 'S'
            else:
                char = str(o)
            center = char.center(j-i-2)
            s+= '[' + center + ']'
        return s

if __name__ == "__main__":
    import string


    trace = ImmutableList(string.ascii_letters)

    trace.remove(13,26)
    assert len(trace) == (len(string.ascii_letters)-13)
    assert trace[13] == 'A'

    trace.remove(26,39)
    assert trace[:13] == list(string.lowercase[:13])
    assert trace[13:] == list(string.uppercase[:13])

    trace.insert(13, "XXX")
    assert trace[13:16] == ['X']*3

    trace.remove(12,16)
    assert len(trace) == 25
    assert trace[12] == 'A'

    # Test out the various slice-based operators
    trace = ImmutableList(string.ascii_letters)

    del trace[13:26]
    assert len(trace) == (len(string.ascii_letters)-13)
    assert trace[13] == 'A'

    del trace[26:39]
    assert trace[:13] == list(string.lowercase[:13])
    assert trace[13:] == list(string.uppercase[:13])

    trace.insert(13, "XXX")
    assert trace[13:16] == ['X']*3

    trace[13:16] = "YYY"
    assert trace[13:16] == ['Y']*3

    trace[14] = "Z"

    bk = list(trace)
    trace.optimize()
    assert bk == list(trace)
    trace.optimize()
    assert bk == list(trace)

    trace.remove(12,16)
    assert len(trace) == 25
    assert trace[12] == 'A'

    trace = ImmutableList(string.ascii_letters)
    assert list(trace) == list('abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ')
    trace[21:43] =                                  'hhhhhhhhhhhhhhhhhhhhhh'
    assert list(trace) == list('abcdefghijklmnopqrstuhhhhhhhhhhhhhhhhhhhhhhRSTUVWXYZ')
    trace[6:34] =                    'RRRRRRRRRRRRRRRRRRRRRRRRRRRR'
    assert list(trace) == list('abcdefRRRRRRRRRRRRRRRRRRRRRRRRRRRRhhhhhhhhhRSTUVWXYZ')
    trace[0:12] =              'RRRRRRRRRRRR'
    assert list(trace) == list('RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRhhhhhhhhhRSTUVWXYZ')
    trace[13:29] =                          'jjjjjjjjjjjjjjjj'
    assert list(trace) == list('RRRRRRRRRRRRRjjjjjjjjjjjjjjjjRRRRRhhhhhhhhhRSTUVWXYZ')
    trace[15:46] =                            'zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz'
    assert list(trace) == list('RRRRRRRRRRRRRjjzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzUVWXYZ')

    from random import randint, choice

    trace = ImmutableList(string.ascii_letters)
    for i in range(100000):
        a,b = randint(0,len(trace)), randint(0,len(trace))
        if a == b: continue
        a,b = sorted((a,b))
        c = choice(string.letters)
        trace[a:b] = c*(b-a)
        assert trace[a:b] == list(c*(b-a))

    print trace.interval_string()
    trace.optimize()
    print trace.interval_string()
