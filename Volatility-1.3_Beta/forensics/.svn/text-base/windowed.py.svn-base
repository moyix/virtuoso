class WindowedAddressSpace:
    """Create an address space that provides a window on
    a larger space."""
    def __init__(self, base, start, size):
        self.base = base
        self.start = start
        self.fsize = size
    def clamp(self, offset, length):
        if offset > self.fsize:
            # Trying to read outside the window
            raise IOError("Out of windowed range")
        
        if offset + length > self.fsize:
            # requested more data than we're willing to provide
            # clamp the length down
            ret_length = self.fsize - offset
        else:
            ret_length = length

        return ret_length
    def read(self, offset, length):
        length = self.clamp(offset, length)
        return self.base.read(self.start + offset, length)
    def zread(self, offset, length):
        length = self.clamp(offset, length)
        return self.base.zread(self.start + offset, length)
