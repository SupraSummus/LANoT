import struct


class InterfaceError(Exception):
    pass


class Interface:
    COMMAND_ECHO = ord('\n')
    COMMAND_OK = ord('k')
    COMMAND_INVALID = ord('?')
    COMMAND_PAGE_SIZE = 12
    COMMAND_PAGE_COUNT = 11

    def __init__(self, input_stream, output_stream):
        self.input_stream = input_stream
        self.output_stream = output_stream

    # ### command handling ###

    def do_echo(self):
        self.write_uint8(self.COMMAND_ECHO)
        self.expect_uint8(self.COMMAND_ECHO)

    def page_size(self):
        self.write_uint8(self.COMMAND_PAGE_SIZE)
        return self.read_uint32()

    def page_count(self):
        self.write_uint8(self.COMMAND_PAGE_COUNT)
        return self.read_uint32()

    # ### write/read of dynamic size

    def write(self, b):
        self.output_stream.write(b)
        self.output_stream.flush()

    def read(self, l):
        return self.input_stream.read(l)

    # ### write/read basic types

    def write_uint8(self, v):
        return self.write(bytes((v,)))

    def read_uint8(self):
        return self.read(1)[0]

    def expect_uint8(self, v):
        reply = self.read_uint8()
        if reply != v:
            raise InterfaceError(f'expected uint8 {v}, got {reply}')

    def write_uint32(self, v):
        return self.write(struct.pack('I', v))

    def read_uint32(self):
        return struct.unpack('I', self.read(4))


if __name__ == "__main__":
    i = Interface(
        input_stream=open(10, 'rb'),
        output_stream=open(11, 'wb'),
    )
    i.do_echo()
    print('echo done')
    print('page_size', i.page_size())
    print('page_count', i.page_count())
    
