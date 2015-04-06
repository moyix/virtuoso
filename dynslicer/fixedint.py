import operator

def _toFixedUnsigned(n, b):
   "Truncate n to a b-bit unsigned long."
   return n & ((1L << b) - 1)    # bitmask of b 1's

def _toFixedSigned(n, b):
   "Truncate n to a b-bit signed long."
   result = _toFixedUnsigned(n, b)
   if result >= (1L << b - 1):   # More than "maxint"?
      result -= 1L << b          # Then wrap around.
   return result

class _Integer(object):
   "Abstract base class for SignedType and UnsignedType."
   # Numeric conversions
   def __long__(self):
      return self._value
   def __int__(self):
      return int(self._value)
   def __float__(self):
      return float(self._value)
   def __nonzero__(self):
      return self._value != 0
   # String conversions.
   def __str__(self):
      return str(self._value)
   # Arithmetic
   def __pos__(self):
      return self
   def __neg__(self):
      return type(self)(-self._value)
   def __add__(self, other):
      if isinstance(other, type(self)):
        return type(self)(self._value + other._value)
      else:
        return type(self)(self._value + self.__class__(other)._value)
   def __rsub__(self, other):
      return self.__class__(other) - self
   def __sub__(self, other):
      if isinstance(other, type(self)):
        return type(self)(self._value - other._value)
      else:
        return type(self)(self._value - self.__class__(other)._value)
   def __mul__(self, other):
      if isinstance(other, type(self)):
        return type(self)(self._value * other._value)
      else:
        return type(self)(self._value * self.__class__(other)._value)
   def __floordiv__(self, other):
      if isinstance(other, type(self)):
        return type(self)(self._value // other._value)
      else:
        return type(self)(self._value // self.__class__(other)._value)
   def __mod__(self, other):
      if isinstance(other, type(self)):
        return type(self)(self._value % other._value)
      else:
        return type(self)(self._value % self.__class__(other)._value)
   def __divmod__(self, other):
      return self // other, self % other
   # Relational
   def __cmp__(self, other):
      return cmp(long(self), other)
   # Bit-bashing
   def __lshift__(self, other):
      if isinstance(other, type(self)):
          return type(self)(self._value << other._value)
      else:
          return type(self)(self._value << self.__class__(other)._value)
   def __rshift__(self, other):
      if isinstance(other, type(self)):
          return type(self)(self._value >> other._value)
      else:
          return type(self)(self._value >> self.__class__(other)._value)
   def __invert__(self):
      return type(self)(~self._value)
   def __and__(self, other):
      if isinstance(other, type(self)):
        return type(self)(self._value & other._value)
      else:
        return type(self)(self._value & self.__class__(other)._value)
   def __or__(self, other):
      if isinstance(other, type(self)):
        return type(self)(self._value | other._value)
      else:
        return type(self)(self._value | self.__class__(other)._value)
   def __ror__(self, other):
      if isinstance(other, type(self)):
        return type(self)(self._value | other._value)
      else:
        return type(self)(self._value | self.__class__(other)._value)
   def __xor__(self, other):
      if isinstance(other, type(self)):
        return type(self)(self._value ^ other._value)
      else:
        return type(self)(self._value ^ self.__class__(other)._value)

_utypes = {}

def UnsignedType(bits):
   "Returns a fixed-width unsigned int type with the given number of bits."
   if bits in _utypes:
      return _utypes[bits]
   else:
      class unsigned(_Integer):
        __doc__ = '%d-bit unsigned integer type' % bits
        def __init__(self, value):
          self._value = _toFixedUnsigned(int(value), bits)
        def __repr__(self):
          return 'fixedint.UnsignedType(%d)(%d)' % (bits, self._value)
      return unsigned

Byte = UnsignedType(8)
UShort = UnsignedType(16)
UInt = UnsignedType(32)
ULong = UnsignedType(64)

_stypes = {}

def SignedType(bits):
   "Returns a fixed-width signed int type with the given number of bits."
   if bits in _stypes:
      return _stypes[bits]
   else:
      class signed(_Integer):
        __doc__ = '%d-bit signed integer type' % bits
        def __init__(self, value):
          self._value = _toFixedSigned(int(value), bits)
        def __repr__(self):
          return 'fixedint.SignedType(%d)(%d)' % (bits, self._value)
      return signed

SByte = SignedType(8)
Short = SignedType(16)
Int = SignedType(32)
Long = SignedType(64)

