from PIL import Image
from math import cos, radians

def image_to_2d_array(filename):
    "Returns array, mode tuple"
    _im = Image.open(filename)
    _im.load()
    _width, _height = _im.size
    return [[_im.getpixel((i, j))
             for i in range(_width)]
            for j in range(_height)], _im.mode


def isographic_to_orthographic(filename, to_filename, match_y=True):
    _pa = PixelArray(*image_to_2d_array(filename))
    _newHeight = 0
    _adjustment = 0
    _height = _pa.height
    while _newHeight != _height:
        _pa.gp_proj()
        _dh = _pa.height - _height
        _pa.vscale(1 - float(_dh)/_pa.height + _adjustment)
        _newHeight = _pa.height
        _adjustment = float(_height - _newHeight) / _height
    _pa.draw().save(to_filename)


class PixelRow(object):
    def __init__(self, pixel_list):
        self.pixels = pixel_list
        self.height = 1.0

        
class PixelArray(object):
    def __init__(self, pixel_list_2d, mode):
        self.mode = mode
        self.rows = [PixelRow(r) for r in pixel_list_2d]

    def vscale(self, scalar):
        for r in self.rows:
            r.height *= scalar

    @property
    def width(self):
        return len(self.rows[0].pixels)

    @property
    def height(self):
        return int(sum([r.height for r in self.rows]))

    def gp_proj(self, scale=2.):
        self.vscale(scale)
        _mid = len(self.rows) / 2
        for i, r in enumerate(self.rows):
            _ri = i - _mid
            _mag = abs(_ri)
            _magT = _mag * (90./_mid)
            self.rows[i].height *= cos(radians(_magT))

    def draw(self):
        "Return a PIL image"
        _img = Image.new(self.mode, (self.width, self.height), "black")
        _pix = _img.load()
        for j in xrange(self.height):
            _ri = 0
            _s = sum([r.height for r in self.rows[:_ri + 1]])
            while _s < (j + 1):
                    _ri += 1
                    _s += self.rows[_ri].height
            for i in xrange(self.width):
                try:
                    _pix[i,j] = self.rows[_ri].pixels[i]
                except:
                    print 'exception', i, j
                    continue
        return _img
            
            
    
