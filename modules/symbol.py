WIDTH = 87
HEIGHT = 87
FIRST = 0x20
LAST = 0x7f

_font =\
b'\x00\x4a\x5a\x02\x44\x60\x44\x52\x60\x52\x02\x44\x60\x44\x60'\
b'\x60\x44\x02\x52\x52\x52\x3e\x52\x66\x02\x44\x60\x44\x44\x60'\
b'\x60\x02\x44\x60\x44\x52\x60\x52\x02\x46\x5e\x46\x59\x5e\x4b'\
b'\x02\x4b\x59\x4b\x5e\x59\x46\x02\x52\x52\x52\x44\x52\x60\x02'\
b'\x4b\x59\x4b\x46\x59\x5e\x02\x46\x5e\x46\x4b\x5e\x59\x02\x4b'\
b'\x59\x4b\x52\x59\x52\x02\x4d\x57\x4d\x57\x57\x4d\x02\x52\x52'\
b'\x52\x4b\x52\x59\x02\x4d\x57\x4d\x4d\x57\x57\x07\x47\x52\x52'\
b'\x47\x50\x47\x4d\x48\x4a\x4a\x48\x4d\x47\x50\x47\x52\x07\x47'\
b'\x52\x47\x52\x47\x54\x48\x57\x4a\x5a\x4d\x5c\x50\x5d\x52\x5d'\
b'\x07\x52\x5d\x52\x5d\x54\x5d\x57\x5c\x5a\x5a\x5c\x57\x5d\x54'\
b'\x5d\x52\x07\x52\x5d\x5d\x52\x5d\x50\x5c\x4d\x5a\x4a\x57\x48'\
b'\x54\x47\x52\x47\x08\x44\x60\x44\x4f\x47\x51\x4b\x53\x50\x54'\
b'\x54\x54\x59\x53\x5d\x51\x60\x4f\x08\x50\x55\x55\x44\x53\x47'\
b'\x51\x4b\x50\x50\x50\x54\x51\x59\x53\x5d\x55\x60\x08\x4f\x54'\
b'\x4f\x44\x51\x47\x53\x4b\x54\x50\x54\x54\x53\x59\x51\x5d\x4f'\
b'\x60\x08\x44\x60\x44\x55\x47\x53\x4b\x51\x50\x50\x54\x50\x59'\
b'\x51\x5d\x53\x60\x55\x04\x4b\x59\x52\x4a\x59\x4e\x4b\x56\x52'\
b'\x5a\x04\x4a\x5a\x4a\x52\x4e\x4b\x56\x59\x5a\x52\x04\x4b\x59'\
b'\x4b\x56\x4b\x4e\x59\x56\x59\x4e\x04\x4a\x5a\x4c\x58\x4a\x50'\
b'\x5a\x54\x58\x4c\x16\x4a\x5a\x4a\x5d\x4c\x5d\x4f\x5c\x51\x5b'\
b'\x54\x58\x55\x56\x56\x53\x56\x4f\x55\x4c\x54\x4a\x53\x49\x51'\
b'\x49\x50\x4a\x4f\x4c\x4e\x4f\x4e\x53\x4f\x56\x50\x58\x53\x5b'\
b'\x55\x5c\x58\x5d\x5a\x5d\x16\x49\x5d\x5d\x5a\x5d\x58\x5c\x55'\
b'\x5b\x53\x58\x50\x56\x4f\x53\x4e\x4f\x4e\x4c\x4f\x4a\x50\x49'\
b'\x51\x49\x53\x4a\x54\x4c\x55\x4f\x56\x53\x56\x56\x55\x58\x54'\
b'\x5b\x51\x5c\x4f\x5d\x4c\x5d\x4a\x16\x4a\x5a\x5a\x47\x58\x47'\
b'\x55\x48\x53\x49\x50\x4c\x4f\x4e\x4e\x51\x4e\x55\x4f\x58\x50'\
b'\x5a\x51\x5b\x53\x5b\x54\x5a\x55\x58\x56\x55\x56\x51\x55\x4e'\
b'\x54\x4c\x51\x49\x4f\x48\x4c\x47\x4a\x47\x16\x47\x5b\x47\x4a'\
b'\x47\x4c\x48\x4f\x49\x51\x4c\x54\x4e\x55\x51\x56\x55\x56\x58'\
b'\x55\x5a\x54\x5b\x53\x5b\x51\x5a\x50\x58\x4f\x55\x4e\x51\x4e'\
b'\x4e\x4f\x4c\x50\x49\x53\x48\x55\x47\x58\x47\x5a\x14\x45\x5b'\
b'\x45\x50\x46\x52\x48\x54\x4a\x55\x4d\x56\x51\x56\x55\x55\x58'\
b'\x53\x5a\x50\x5b\x4e\x5a\x4c\x57\x4c\x53\x4d\x51\x4e\x4e\x50'\
b'\x4c\x53\x4b\x56\x4b\x59\x4c\x5c\x4d\x5e\x12\x45\x59\x45\x54'\
b'\x48\x56\x4b\x57\x50\x57\x53\x56\x56\x54\x58\x51\x59\x4e\x59'\
b'\x4c\x58\x4b\x56\x4b\x53\x4c\x50\x4e\x4e\x51\x4d\x54\x4d\x59'\
b'\x4e\x5c\x50\x5f\x19\x4f\x55\x51\x4f\x4f\x51\x4f\x53\x51\x55'\
b'\x53\x55\x55\x53\x55\x51\x53\x4f\x51\x4f\x20\x52\x51\x50\x50'\
b'\x51\x50\x53\x51\x54\x53\x54\x54\x53\x54\x51\x53\x50\x51\x50'\
b'\x20\x52\x52\x51\x51\x52\x52\x53\x53\x52\x52\x51\x0a\x52\x57'\
b'\x52\x4d\x53\x4d\x55\x4e\x56\x4f\x57\x51\x57\x53\x56\x55\x55'\
b'\x56\x53\x57\x52\x57\x08\x44\x60\x44\x52\x4a\x52\x20\x52\x4f'\
b'\x52\x55\x52\x20\x52\x5a\x52\x60\x52\x04\x44\x60\x44\x55\x44'\
b'\x4f\x60\x4f\x60\x55\x05\x4a\x5a\x52\x44\x4a\x52\x20\x52\x52'\
b'\x44\x5a\x52\x08\x44\x60\x44\x52\x60\x52\x20\x52\x4a\x59\x5a'\
b'\x59\x20\x52\x50\x60\x54\x60\x08\x44\x60\x44\x52\x60\x52\x20'\
b'\x52\x44\x52\x52\x62\x20\x52\x60\x52\x52\x62\x11\x4b\x59\x51'\
b'\x4b\x4e\x4c\x4c\x4e\x4b\x51\x4b\x53\x4c\x56\x4e\x58\x51\x59'\
b'\x53\x59\x56\x58\x58\x56\x59\x53\x59\x51\x58\x4e\x56\x4c\x53'\
b'\x4b\x51\x4b\x05\x4c\x58\x4c\x4c\x4c\x58\x58\x58\x58\x4c\x4c'\
b'\x4c\x04\x4b\x59\x52\x4a\x4b\x56\x59\x56\x52\x4a\x05\x4c\x58'\
b'\x52\x48\x4c\x52\x52\x5c\x58\x52\x52\x48\x0b\x4a\x5a\x52\x49'\
b'\x50\x4f\x4a\x4f\x4f\x53\x4d\x59\x52\x55\x57\x59\x55\x53\x5a'\
b'\x4f\x54\x4f\x52\x49\x05\x4b\x59\x52\x4b\x52\x59\x20\x52\x4b'\
b'\x52\x59\x52\x05\x4d\x57\x4d\x4d\x57\x57\x20\x52\x57\x4d\x4d'\
b'\x57\x08\x4d\x57\x52\x4c\x52\x58\x20\x52\x4d\x4f\x57\x55\x20'\
b'\x52\x57\x4f\x4d\x55\x22\x4e\x56\x51\x4e\x4f\x4f\x4e\x51\x4e'\
b'\x53\x4f\x55\x51\x56\x53\x56\x55\x55\x56\x53\x56\x51\x55\x4f'\
b'\x53\x4e\x51\x4e\x20\x52\x4f\x51\x4f\x53\x20\x52\x50\x50\x50'\
b'\x54\x20\x52\x51\x4f\x51\x55\x20\x52\x52\x4f\x52\x55\x20\x52'\
b'\x53\x4f\x53\x55\x20\x52\x54\x50\x54\x54\x20\x52\x55\x51\x55'\
b'\x53\x1a\x4e\x56\x4e\x4e\x4e\x56\x56\x56\x56\x4e\x4e\x4e\x20'\
b'\x52\x4f\x4f\x4f\x55\x20\x52\x50\x4f\x50\x55\x20\x52\x51\x4f'\
b'\x51\x55\x20\x52\x52\x4f\x52\x55\x20\x52\x53\x4f\x53\x55\x20'\
b'\x52\x54\x4f\x54\x55\x20\x52\x55\x4f\x55\x55\x10\x4d\x57\x52'\
b'\x4c\x4d\x55\x57\x55\x52\x4c\x20\x52\x52\x4f\x4f\x54\x20\x52'\
b'\x52\x4f\x55\x54\x20\x52\x52\x52\x51\x54\x20\x52\x52\x52\x53'\
b'\x54\x10\x4c\x55\x4c\x52\x55\x57\x55\x4d\x4c\x52\x20\x52\x4f'\
b'\x52\x54\x55\x20\x52\x4f\x52\x54\x4f\x20\x52\x52\x52\x54\x53'\
b'\x20\x52\x52\x52\x54\x51\x10\x4d\x57\x52\x58\x57\x4f\x4d\x4f'\
b'\x52\x58\x20\x52\x52\x55\x55\x50\x20\x52\x52\x55\x4f\x50\x20'\
b'\x52\x52\x52\x53\x50\x20\x52\x52\x52\x51\x50\x10\x4f\x58\x58'\
b'\x52\x4f\x4d\x4f\x57\x58\x52\x20\x52\x55\x52\x50\x4f\x20\x52'\
b'\x55\x52\x50\x55\x20\x52\x52\x52\x50\x51\x20\x52\x52\x52\x50'\
b'\x53\x0a\x52\x59\x52\x4b\x52\x59\x20\x52\x52\x4b\x59\x4e\x52'\
b'\x51\x20\x52\x53\x4d\x56\x4e\x53\x4f\x14\x49\x5b\x52\x47\x52'\
b'\x56\x20\x52\x4d\x4a\x57\x50\x20\x52\x57\x4a\x4d\x50\x20\x52'\
b'\x49\x56\x4c\x5c\x20\x52\x5b\x56\x58\x5c\x20\x52\x49\x56\x5b'\
b'\x56\x20\x52\x4c\x5c\x58\x5c\x0c\x4d\x57\x52\x4c\x52\x58\x20'\
b'\x52\x4f\x4f\x55\x4f\x20\x52\x4d\x55\x4f\x57\x51\x58\x53\x58'\
b'\x55\x57\x57\x55\x0a\x4c\x58\x52\x4c\x52\x58\x20\x52\x4c\x51'\
b'\x4d\x4f\x57\x4f\x58\x51\x20\x52\x50\x57\x54\x57\x0d\x4b\x59'\
b'\x4d\x4e\x57\x58\x20\x52\x57\x4e\x4d\x58\x20\x52\x4f\x4c\x4c'\
b'\x4f\x4b\x51\x20\x52\x55\x4c\x58\x4f\x59\x51\x11\x49\x5b\x4e'\
b'\x49\x49\x5b\x20\x52\x56\x49\x5b\x5b\x20\x52\x4d\x4d\x5b\x5b'\
b'\x20\x52\x57\x4d\x49\x5b\x20\x52\x4e\x49\x56\x49\x20\x52\x4d'\
b'\x4d\x57\x4d\x02\x4b\x59\x4b\x46\x59\x5e\x0a\x47\x5b\x4d\x4a'\
b'\x53\x56\x20\x52\x4b\x50\x53\x4c\x20\x52\x47\x5c\x5b\x5c\x5b'\
b'\x52\x47\x5c\x0d\x4c\x58\x50\x4c\x50\x50\x4c\x50\x4c\x54\x50'\
b'\x54\x50\x58\x54\x58\x54\x54\x58\x54\x58\x50\x54\x50\x54\x4c'\
b'\x50\x4c\x1f\x4b\x59\x59\x50\x58\x4e\x56\x4c\x53\x4b\x51\x4b'\
b'\x4e\x4c\x4c\x4e\x4b\x51\x4b\x53\x4c\x56\x4e\x58\x51\x59\x53'\
b'\x59\x56\x58\x58\x56\x59\x54\x20\x52\x59\x50\x57\x4e\x55\x4d'\
b'\x53\x4d\x51\x4e\x50\x4f\x4f\x51\x4f\x53\x50\x55\x51\x56\x53'\
b'\x57\x55\x57\x57\x56\x59\x54\x09\x4b\x59\x52\x4a\x4b\x56\x59'\
b'\x56\x52\x4a\x20\x52\x52\x5a\x59\x4e\x4b\x4e\x52\x5a\x21\x47'\
b'\x5d\x50\x49\x50\x47\x51\x46\x53\x46\x54\x47\x54\x49\x20\x52'\
b'\x47\x5a\x48\x58\x4a\x56\x4b\x54\x4c\x50\x4c\x4b\x4d\x4a\x4f'\
b'\x49\x55\x49\x57\x4a\x58\x4b\x58\x50\x59\x54\x5a\x56\x5c\x58'\
b'\x5d\x5a\x20\x52\x47\x5a\x5d\x5a\x20\x52\x51\x5a\x50\x5b\x51'\
b'\x5c\x53\x5c\x54\x5b\x53\x5a\x3f\x4a\x5a\x52\x4d\x52\x53\x20'\
b'\x52\x52\x53\x51\x5c\x20\x52\x52\x53\x53\x5c\x20\x52\x51\x5c'\
b'\x53\x5c\x20\x52\x52\x4d\x51\x4a\x50\x48\x4e\x47\x20\x52\x51'\
b'\x4a\x4e\x47\x20\x52\x52\x4d\x53\x4a\x54\x48\x56\x47\x20\x52'\
b'\x53\x4a\x56\x47\x20\x52\x52\x4d\x4e\x4b\x4c\x4b\x4a\x4d\x20'\
b'\x52\x50\x4c\x4c\x4c\x4a\x4d\x20\x52\x52\x4d\x56\x4b\x58\x4b'\
b'\x5a\x4d\x20\x52\x54\x4c\x58\x4c\x5a\x4d\x20\x52\x52\x4d\x50'\
b'\x4e\x4f\x4f\x4f\x52\x20\x52\x52\x4d\x50\x4f\x4f\x52\x20\x52'\
b'\x52\x4d\x54\x4e\x55\x4f\x55\x52\x20\x52\x52\x4d\x54\x4f\x55'\
b'\x52\x5d\x4a\x5a\x52\x49\x52\x4b\x20\x52\x52\x4e\x52\x50\x20'\
b'\x52\x52\x53\x52\x55\x20\x52\x52\x59\x51\x5c\x20\x52\x52\x59'\
b'\x53\x5c\x20\x52\x51\x5c\x53\x5c\x20\x52\x52\x47\x51\x49\x50'\
b'\x4a\x20\x52\x52\x47\x53\x49\x54\x4a\x20\x52\x50\x4a\x52\x49'\
b'\x54\x4a\x20\x52\x52\x4b\x50\x4e\x4e\x4f\x4d\x4e\x20\x52\x52'\
b'\x4b\x54\x4e\x56\x4f\x57\x4e\x20\x52\x4e\x4f\x50\x4f\x52\x4e'\
b'\x54\x4f\x56\x4f\x20\x52\x52\x50\x50\x53\x4e\x54\x4c\x54\x4b'\
b'\x52\x4b\x53\x4c\x54\x20\x52\x52\x50\x54\x53\x56\x54\x58\x54'\
b'\x59\x52\x59\x53\x58\x54\x20\x52\x4e\x54\x50\x54\x52\x53\x54'\
b'\x54\x56\x54\x20\x52\x52\x55\x50\x58\x4f\x59\x4d\x5a\x4c\x5a'\
b'\x4b\x59\x4a\x57\x4a\x59\x4c\x5a\x20\x52\x52\x55\x54\x58\x55'\
b'\x59\x57\x5a\x58\x5a\x59\x59\x5a\x57\x5a\x59\x58\x5a\x20\x52'\
b'\x4d\x5a\x4f\x5a\x52\x59\x55\x5a\x57\x5a\x27\x4a\x5a\x52\x59'\
b'\x51\x5c\x20\x52\x52\x59\x53\x5c\x20\x52\x51\x5c\x53\x5c\x20'\
b'\x52\x52\x59\x55\x5a\x58\x5a\x5a\x58\x5a\x55\x59\x54\x57\x54'\
b'\x59\x52\x5a\x4f\x59\x4d\x57\x4c\x55\x4d\x56\x4a\x55\x48\x53'\
b'\x47\x51\x47\x4f\x48\x4e\x4a\x4f\x4d\x4d\x4c\x4b\x4d\x4a\x4f'\
b'\x4b\x52\x4d\x54\x4b\x54\x4a\x55\x4a\x58\x4c\x5a\x4f\x5a\x52'\
b'\x59\x1f\x4a\x5a\x52\x59\x51\x5c\x20\x52\x52\x59\x53\x5c\x20'\
b'\x52\x51\x5c\x53\x5c\x20\x52\x52\x59\x56\x58\x56\x56\x58\x55'\
b'\x58\x52\x5a\x51\x5a\x4c\x59\x49\x58\x48\x56\x48\x54\x47\x50'\
b'\x47\x4e\x48\x4c\x48\x4b\x49\x4a\x4c\x4a\x51\x4c\x52\x4c\x55'\
b'\x4e\x56\x4e\x58\x52\x59\x0e\x49\x5b\x49\x50\x4b\x52\x20\x52'\
b'\x4c\x4b\x4e\x50\x20\x52\x52\x47\x52\x4f\x20\x52\x58\x4b\x56'\
b'\x50\x20\x52\x5b\x50\x59\x52\x1b\x47\x5d\x49\x49\x4a\x4b\x4b'\
b'\x4f\x4b\x55\x4a\x59\x49\x5b\x20\x52\x5b\x49\x5a\x4b\x59\x4f'\
b'\x59\x55\x5a\x59\x5b\x5b\x20\x52\x49\x49\x4b\x4a\x4f\x4b\x55'\
b'\x4b\x59\x4a\x5b\x49\x20\x52\x49\x5b\x4b\x5a\x4f\x59\x55\x59'\
b'\x59\x5a\x5b\x5b\x36\x46\x5e\x52\x52\x52\x5b\x51\x5c\x20\x52'\
b'\x52\x56\x51\x5c\x20\x52\x52\x49\x51\x48\x4f\x48\x4e\x49\x4e'\
b'\x4b\x4f\x4e\x52\x52\x20\x52\x52\x49\x53\x48\x55\x48\x56\x49'\
b'\x56\x4b\x55\x4e\x52\x52\x20\x52\x52\x52\x4e\x4f\x4c\x4e\x4a'\
b'\x4e\x49\x4f\x49\x51\x4a\x52\x20\x52\x52\x52\x56\x4f\x58\x4e'\
b'\x5a\x4e\x5b\x4f\x5b\x51\x5a\x52\x20\x52\x52\x52\x4e\x55\x4c'\
b'\x56\x4a\x56\x49\x55\x49\x53\x4a\x52\x20\x52\x52\x52\x56\x55'\
b'\x58\x56\x5a\x56\x5b\x55\x5b\x53\x5a\x52\x2d\x4a\x5a\x55\x49'\
b'\x54\x4a\x55\x4b\x56\x4a\x56\x49\x55\x47\x53\x46\x51\x46\x4f'\
b'\x47\x4e\x49\x4e\x4b\x4f\x4d\x51\x4f\x56\x52\x20\x52\x4f\x4d'\
b'\x54\x50\x56\x52\x57\x54\x57\x56\x56\x58\x54\x5a\x20\x52\x50'\
b'\x4e\x4e\x50\x4d\x52\x4d\x54\x4e\x56\x50\x58\x55\x5b\x20\x52'\
b'\x4e\x56\x53\x59\x55\x5b\x56\x5d\x56\x5f\x55\x61\x53\x62\x51'\
b'\x62\x4f\x61\x4e\x5f\x4e\x5e\x4f\x5d\x50\x5e\x4f\x5f\x1d\x4a'\
b'\x5a\x52\x46\x51\x48\x52\x4a\x53\x48\x52\x46\x20\x52\x52\x46'\
b'\x52\x62\x20\x52\x52\x51\x51\x54\x52\x62\x53\x54\x52\x51\x20'\
b'\x52\x4c\x4d\x4e\x4e\x50\x4d\x4e\x4c\x4c\x4d\x20\x52\x4c\x4d'\
b'\x58\x4d\x20\x52\x54\x4d\x56\x4e\x58\x4d\x56\x4c\x54\x4d\x37'\
b'\x4a\x5a\x52\x46\x51\x48\x52\x4a\x53\x48\x52\x46\x20\x52\x52'\
b'\x46\x52\x54\x20\x52\x52\x50\x51\x52\x53\x56\x52\x58\x51\x56'\
b'\x53\x52\x52\x50\x20\x52\x52\x54\x52\x62\x20\x52\x52\x5e\x51'\
b'\x60\x52\x62\x53\x60\x52\x5e\x20\x52\x4c\x4d\x4e\x4e\x50\x4d'\
b'\x4e\x4c\x4c\x4d\x20\x52\x4c\x4d\x58\x4d\x20\x52\x54\x4d\x56'\
b'\x4e\x58\x4d\x56\x4c\x54\x4d\x20\x52\x4c\x5b\x4e\x5c\x50\x5b'\
b'\x4e\x5a\x4c\x5b\x20\x52\x4c\x5b\x58\x5b\x20\x52\x54\x5b\x56'\
b'\x5c\x58\x5b\x56\x5a\x54\x5b\x11\x45\x5f\x52\x49\x51\x4a\x52'\
b'\x4b\x53\x4a\x52\x49\x20\x52\x49\x59\x48\x5a\x49\x5b\x4a\x5a'\
b'\x49\x59\x20\x52\x5b\x59\x5a\x5a\x5b\x5b\x5c\x5a\x5b\x59\x20'\
b'\x46\x5e\x52\x48\x4e\x4c\x4b\x50\x4a\x53\x4a\x55\x4b\x57\x4d'\
b'\x58\x4f\x58\x51\x57\x52\x55\x20\x52\x52\x48\x56\x4c\x59\x50'\
b'\x5a\x53\x5a\x55\x59\x57\x57\x58\x55\x58\x53\x57\x52\x55\x20'\
b'\x52\x52\x55\x51\x59\x50\x5c\x20\x52\x52\x55\x53\x59\x54\x5c'\
b'\x20\x52\x50\x5c\x54\x5c\x19\x46\x5e\x52\x4e\x51\x4b\x50\x49'\
b'\x4e\x48\x4d\x48\x4b\x49\x4a\x4b\x4a\x4f\x4b\x52\x4c\x54\x4e'\
b'\x57\x52\x5c\x20\x52\x52\x4e\x53\x4b\x54\x49\x56\x48\x57\x48'\
b'\x59\x49\x5a\x4b\x5a\x4f\x59\x52\x58\x54\x56\x57\x52\x5c\x13'\
b'\x46\x5e\x52\x47\x50\x4a\x4c\x4f\x49\x52\x20\x52\x52\x47\x54'\
b'\x4a\x58\x4f\x5b\x52\x20\x52\x49\x52\x4c\x55\x50\x5a\x52\x5d'\
b'\x20\x52\x5b\x52\x58\x55\x54\x5a\x52\x5d\x2f\x46\x5e\x52\x54'\
b'\x54\x57\x56\x58\x58\x58\x5a\x57\x5b\x55\x5b\x53\x5a\x51\x58'\
b'\x50\x56\x50\x53\x51\x20\x52\x53\x51\x55\x4f\x56\x4d\x56\x4b'\
b'\x55\x49\x53\x48\x51\x48\x4f\x49\x4e\x4b\x4e\x4d\x4f\x4f\x51'\
b'\x51\x20\x52\x51\x51\x4e\x50\x4c\x50\x4a\x51\x49\x53\x49\x55'\
b'\x4a\x57\x4c\x58\x4e\x58\x50\x57\x52\x54\x20\x52\x52\x54\x51'\
b'\x59\x50\x5c\x20\x52\x52\x54\x53\x59\x54\x5c\x20\x52\x50\x5c'\
b'\x54\x5c\x2f\x49\x5b\x56\x2b\x53\x2d\x51\x2f\x50\x31\x4f\x34'\
b'\x4f\x38\x50\x3c\x54\x44\x55\x47\x55\x4a\x54\x4d\x52\x50\x20'\
b'\x52\x53\x2d\x51\x30\x50\x34\x50\x38\x51\x3b\x55\x43\x56\x47'\
b'\x56\x4a\x55\x4d\x52\x50\x4e\x52\x52\x54\x55\x57\x56\x5a\x56'\
b'\x5d\x55\x61\x51\x69\x50\x6c\x50\x70\x51\x74\x53\x77\x20\x52'\
b'\x52\x54\x54\x57\x55\x5a\x55\x5d\x54\x60\x50\x68\x4f\x6c\x4f'\
b'\x70\x50\x73\x51\x75\x53\x77\x56\x79\x2f\x49\x5b\x4e\x2b\x51'\
b'\x2d\x53\x2f\x54\x31\x55\x34\x55\x38\x54\x3c\x50\x44\x4f\x47'\
b'\x4f\x4a\x50\x4d\x52\x50\x20\x52\x51\x2d\x53\x30\x54\x34\x54'\
b'\x38\x53\x3b\x4f\x43\x4e\x47\x4e\x4a\x4f\x4d\x52\x50\x56\x52'\
b'\x52\x54\x4f\x57\x4e\x5a\x4e\x5d\x4f\x61\x53\x69\x54\x6c\x54'\
b'\x70\x53\x74\x51\x77\x20\x52\x52\x54\x50\x57\x4f\x5a\x4f\x5d'\
b'\x50\x60\x54\x68\x55\x6c\x55\x70\x54\x73\x53\x75\x51\x77\x4e'\
b'\x79\x1f\x49\x5b\x56\x2e\x53\x31\x51\x34\x4f\x38\x4e\x3d\x4e'\
b'\x43\x4f\x49\x50\x4d\x53\x58\x54\x5c\x55\x62\x55\x67\x54\x6c'\
b'\x53\x6f\x51\x73\x20\x52\x53\x31\x51\x35\x50\x38\x4f\x3d\x4f'\
b'\x42\x50\x48\x51\x4c\x54\x57\x55\x5b\x56\x61\x56\x67\x55\x6c'\
b'\x53\x70\x51\x73\x4e\x76\x1f\x49\x5b\x4e\x2e\x51\x31\x53\x34'\
b'\x55\x38\x56\x3d\x56\x43\x55\x49\x54\x4d\x51\x58\x50\x5c\x4f'\
b'\x62\x4f\x67\x50\x6c\x51\x6f\x53\x73\x20\x52\x51\x31\x53\x35'\
b'\x54\x38\x55\x3d\x55\x42\x54\x48\x53\x4c\x50\x57\x4f\x5b\x4e'\
b'\x61\x4e\x67\x4f\x6c\x51\x70\x53\x73\x56\x76\x0d\x37\x5a\x3a'\
b'\x52\x41\x52\x52\x6f\x20\x52\x40\x52\x51\x6f\x20\x52\x3f\x52'\
b'\x52\x72\x20\x52\x5a\x22\x56\x4a\x52\x72\x1a\x49\x5b\x54\x4d'\
b'\x56\x4e\x58\x50\x58\x4f\x57\x4e\x54\x4d\x51\x4d\x4e\x4e\x4d'\
b'\x4f\x4c\x51\x4c\x53\x4d\x55\x4f\x57\x53\x5a\x20\x52\x51\x4d'\
b'\x4f\x4e\x4e\x4f\x4d\x51\x4d\x53\x4e\x55\x53\x5a\x54\x5c\x54'\
b'\x5e\x53\x5f\x51\x5f\x2c\x47\x5d\x4c\x4d\x4b\x4e\x4a\x50\x4a'\
b'\x52\x4b\x55\x4f\x59\x50\x5b\x20\x52\x4a\x52\x4b\x54\x4f\x58'\
b'\x50\x5b\x50\x5d\x4f\x60\x4d\x62\x4c\x62\x4b\x61\x4a\x5f\x4a'\
b'\x5c\x4b\x58\x4d\x54\x4f\x51\x52\x4e\x54\x4d\x56\x4d\x59\x4e'\
b'\x5a\x50\x5a\x54\x59\x58\x57\x5a\x55\x5b\x54\x5b\x53\x5a\x53'\
b'\x58\x54\x57\x55\x58\x54\x59\x20\x52\x56\x4d\x58\x4e\x59\x50'\
b'\x59\x54\x58\x58\x57\x5a\x44\x45\x5f\x59\x47\x58\x48\x59\x49'\
b'\x5a\x48\x59\x47\x57\x46\x54\x46\x51\x47\x4f\x49\x4e\x4b\x4d'\
b'\x4e\x4c\x52\x4a\x5b\x49\x5f\x48\x61\x20\x52\x54\x46\x52\x47'\
b'\x50\x49\x4f\x4b\x4e\x4e\x4c\x57\x4b\x5b\x4a\x5e\x49\x60\x48'\
b'\x61\x46\x62\x44\x62\x43\x61\x43\x60\x44\x5f\x45\x60\x44\x61'\
b'\x20\x52\x5f\x47\x5e\x48\x5f\x49\x60\x48\x60\x47\x5f\x46\x5d'\
b'\x46\x5b\x47\x5a\x48\x59\x4a\x58\x4d\x55\x5b\x54\x5f\x53\x61'\
b'\x20\x52\x5d\x46\x5b\x48\x5a\x4a\x59\x4e\x57\x57\x56\x5b\x55'\
b'\x5e\x54\x60\x53\x61\x51\x62\x4f\x62\x4e\x61\x4e\x60\x4f\x5f'\
b'\x50\x60\x4f\x61\x20\x52\x49\x4d\x5e\x4d\x33\x46\x5e\x5b\x47'\
b'\x5a\x48\x5b\x49\x5c\x48\x5b\x47\x58\x46\x55\x46\x52\x47\x50'\
b'\x49\x4f\x4b\x4e\x4e\x4d\x52\x4b\x5b\x4a\x5f\x49\x61\x20\x52'\
b'\x55\x46\x53\x47\x51\x49\x50\x4b\x4f\x4e\x4d\x57\x4c\x5b\x4b'\
b'\x5e\x4a\x60\x49\x61\x47\x62\x45\x62\x44\x61\x44\x60\x45\x5f'\
b'\x46\x60\x45\x61\x20\x52\x59\x4d\x57\x54\x56\x58\x56\x5a\x57'\
b'\x5b\x5a\x5b\x5c\x59\x5d\x57\x20\x52\x5a\x4d\x58\x54\x57\x58'\
b'\x57\x5a\x58\x5b\x20\x52\x4a\x4d\x5a\x4d\x35\x46\x5e\x59\x47'\
b'\x58\x48\x59\x49\x5a\x48\x5a\x47\x58\x46\x20\x52\x5c\x46\x55'\
b'\x46\x52\x47\x50\x49\x4f\x4b\x4e\x4e\x4d\x52\x4b\x5b\x4a\x5f'\
b'\x49\x61\x20\x52\x55\x46\x53\x47\x51\x49\x50\x4b\x4f\x4e\x4d'\
b'\x57\x4c\x5b\x4b\x5e\x4a\x60\x49\x61\x47\x62\x45\x62\x44\x61'\
b'\x44\x60\x45\x5f\x46\x60\x45\x61\x20\x52\x5b\x46\x57\x54\x56'\
b'\x58\x56\x5a\x57\x5b\x5a\x5b\x5c\x59\x5d\x57\x20\x52\x5c\x46'\
b'\x58\x54\x57\x58\x57\x5a\x58\x5b\x20\x52\x4a\x4d\x59\x4d\x55'\
b'\x40\x63\x54\x47\x53\x48\x54\x49\x55\x48\x54\x47\x52\x46\x4f'\
b'\x46\x4c\x47\x4a\x49\x49\x4b\x48\x4e\x47\x52\x45\x5b\x44\x5f'\
b'\x43\x61\x20\x52\x4f\x46\x4d\x47\x4b\x49\x4a\x4b\x49\x4e\x47'\
b'\x57\x46\x5b\x45\x5e\x44\x60\x43\x61\x41\x62\x3f\x62\x3e\x61'\
b'\x3e\x60\x3f\x5f\x40\x60\x3f\x61\x20\x52\x60\x47\x5f\x48\x60'\
b'\x49\x61\x48\x60\x47\x5d\x46\x5a\x46\x57\x47\x55\x49\x54\x4b'\
b'\x53\x4e\x52\x52\x50\x5b\x4f\x5f\x4e\x61\x20\x52\x5a\x46\x58'\
b'\x47\x56\x49\x55\x4b\x54\x4e\x52\x57\x51\x5b\x50\x5e\x4f\x60'\
b'\x4e\x61\x4c\x62\x4a\x62\x49\x61\x49\x60\x4a\x5f\x4b\x60\x4a'\
b'\x61\x20\x52\x5e\x4d\x5c\x54\x5b\x58\x5b\x5a\x5c\x5b\x5f\x5b'\
b'\x61\x59\x62\x57\x20\x52\x5f\x4d\x5d\x54\x5c\x58\x5c\x5a\x5d'\
b'\x5b\x20\x52\x44\x4d\x5f\x4d\x57\x40\x63\x54\x47\x53\x48\x54'\
b'\x49\x55\x48\x54\x47\x52\x46\x4f\x46\x4c\x47\x4a\x49\x49\x4b'\
b'\x48\x4e\x47\x52\x45\x5b\x44\x5f\x43\x61\x20\x52\x4f\x46\x4d'\
b'\x47\x4b\x49\x4a\x4b\x49\x4e\x47\x57\x46\x5b\x45\x5e\x44\x60'\
b'\x43\x61\x41\x62\x3f\x62\x3e\x61\x3e\x60\x3f\x5f\x40\x60\x3f'\
b'\x61\x20\x52\x5e\x47\x5d\x48\x5e\x49\x5f\x48\x5f\x47\x5d\x46'\
b'\x20\x52\x61\x46\x5a\x46\x57\x47\x55\x49\x54\x4b\x53\x4e\x52'\
b'\x52\x50\x5b\x4f\x5f\x4e\x61\x20\x52\x5a\x46\x58\x47\x56\x49'\
b'\x55\x4b\x54\x4e\x52\x57\x51\x5b\x50\x5e\x4f\x60\x4e\x61\x4c'\
b'\x62\x4a\x62\x49\x61\x49\x60\x4a\x5f\x4b\x60\x4a\x61\x20\x52'\
b'\x60\x46\x5c\x54\x5b\x58\x5b\x5a\x5c\x5b\x5f\x5b\x61\x59\x62'\
b'\x57\x20\x52\x61\x46\x5d\x54\x5c\x58\x5c\x5a\x5d\x5b\x20\x52'\
b'\x44\x4d\x5e\x4d\x13\x4c\x59\x4d\x51\x4e\x4f\x50\x4d\x53\x4d'\
b'\x54\x4e\x54\x51\x52\x57\x52\x5a\x53\x5b\x20\x52\x52\x4d\x53'\
b'\x4e\x53\x51\x51\x57\x51\x5a\x52\x5b\x55\x5b\x57\x59\x58\x57'\
b'\x15\x4c\x58\x52\x4c\x4e\x57\x58\x50\x4c\x50\x56\x57\x52\x4c'\
b'\x20\x52\x52\x52\x52\x4c\x20\x52\x52\x52\x4c\x50\x20\x52\x52'\
b'\x52\x4e\x57\x20\x52\x52\x52\x56\x57\x20\x52\x52\x52\x58\x50'\
b'\x17\x46\x5e\x49\x55\x49\x53\x4a\x50\x4c\x4f\x4e\x4f\x50\x50'\
b'\x54\x53\x56\x54\x58\x54\x5a\x53\x5b\x51\x20\x52\x49\x53\x4a'\
b'\x51\x4c\x50\x4e\x50\x50\x51\x54\x54\x56\x55\x58\x55\x5a\x54'\
b'\x5b\x51\x5b\x4f'

_index =\
b'\x00\x00\x03\x00\x0a\x00\x11\x00\x18\x00\x1f\x00\x26\x00\x2d'\
b'\x00\x34\x00\x3b\x00\x42\x00\x49\x00\x50\x00\x57\x00\x5e\x00'\
b'\x65\x00\x76\x00\x87\x00\x98\x00\xa9\x00\xbc\x00\xcf\x00\xe2'\
b'\x00\xf5\x00\x00\x01\x0b\x01\x16\x01\x21\x01\x50\x01\x7f\x01'\
b'\xae\x01\xdd\x01\x08\x02\x2f\x02\x64\x02\x7b\x02\x8e\x02\x99'\
b'\x02\xa6\x02\xb9\x02\xcc\x02\xf1\x02\xfe\x02\x09\x03\x16\x03'\
b'\x2f\x03\x3c\x03\x49\x03\x5c\x03\xa3\x03\xda\x03\xfd\x03\x20'\
b'\x04\x43\x04\x66\x04\x7d\x04\xa8\x04\xc3\x04\xda\x04\xf7\x04'\
b'\x1c\x05\x23\x05\x3a\x05\x57\x05\x98\x05\xad\x05\xf2\x05\x73'\
b'\x06\x30\x07\x81\x07\xc2\x07\xe1\x07\x1a\x08\x89\x08\xe6\x08'\
b'\x23\x09\x94\x09\xb9\x09\xfc\x09\x31\x0a\x5a\x0a\xbb\x0a\x1c'\
b'\x0b\x7d\x0b\xbe\x0b\xff\x0b\x1c\x0c\x53\x0c\xae\x0c\x39\x0d'\
b'\xa2\x0d\x0f\x0e\xbc\x0e\x6d\x0f\x96\x0f\xc3\x0f'

INDEX = memoryview(_index)
FONT = memoryview(_font)
