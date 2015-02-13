import os, stat, time, ntpath
import tornado.web

class DownHandler(tornado.web.RequestHandler):
    def get(self):
        fname = self.get_argument('dir', default = '')
        try:
            stats = os.stat(fname)
            if not stat.S_ISREG(stats.st_mode):
                raise tornado.web.HTTPError(403)
            path, name = ntpath.split(fname)
            self.content_type = 'application/octet-stream'
            self.write(open(fname).read())
            self.set_header("Content-Type", "application/octet-stream") 
            self.set_header('Content-Disposition', 'attachment; filename="%s"' % name)
        except:
            raise tornado.web.HTTPError(400)

