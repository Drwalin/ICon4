def Settings( **kwargs ):
  return {
    'flags': ['-x', 'c++', '-Wall', '-pedantic', '-Isrc',
    '-std=c++20', '-I/usr/include', '-I./uSockets/src'],
  }
