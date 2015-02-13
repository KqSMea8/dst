import os, mimetypes, cgi
import tornado.web

class CatHandler(tornado.web.RequestHandler):
    def get(self, uri):
        self.write("<h2>Show File Content: %s</h2>" % uri)
        offset = int(self.get_argument("offset", default = '0'))
        block = int(self.get_argument("block", default = '4096'))

        try:
            mfile = os.open(uri, os.O_RDONLY)
            if offset < 0:
                stats = os.stat(uri)
                offset = stats.st_size - block + 1
                if offset < 0:
                    offset = 0
                os.lseek(mfile, offset, os.SEEK_SET)
            else:
                os.lseek(mfile, offset, os.SEEK_SET)
            self.write('<pre>')
            content = os.read(mfile, block)
            self.write(cgi.escape(content))
            self.write('</pre>')


            if offset > block:
                self.write('<a href="/cat/%s">Head</a> | ' % uri)
                self.write('<a href="/cat/%s?offset=%d">Prev %d Bytes</a> | ' %
                        (uri, offset - block - 1, block))
            if len(content) == block:
                self.write('<a href="/cat/%s?offset=%d">Next %d Bytes</a> | ' % 
                        (uri, offset + block + 1, block))
                self.write('<a href="/cat/%s?offset=%d">Tail</a>' % (uri, -1))

        except Exception, e:
            logging.error('cat error: %s' % str(e))
            raise tornado.web.HTTPError(404)
