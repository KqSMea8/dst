import os, stat, time, logging, math
import tornado.web

class ListHandler(tornado.web.RequestHandler):
    def get(self, peid):
        try:
            dir = self.get_argument('dir', default = '.')
            self.write("<h2>List Files For PE: %s</h2>" % (peid))
            if not dir.startswith('.'):
                raise tornado.web.HTTPError(400)
            elif dir == '.':
                files = [f for f in os.listdir(".") if peid in f]
            else:
                files = [f for f in os.listdir(dir)]

            self.write("<ul>")
            for item in files:
                fname = dir + '/' + item
                stats = os.stat(fname)
                self.write("<li><a href='%s'>%s</a> %dKB %s %s</li>" % (
                         self.getUrl(stats, fname, peid), fname, 
                         self.getSize(stats),
                         self.getDownload(stats, fname),
                         self.getMtime(stats)))
            self.write("</ul>")
        except Exception, e:
            logging.error(str(e))
            raise tornado.web.HTTPError(400)

    def getUrl(self, fstat, fname, peid):
        if stat.S_ISDIR(fstat.st_mode):
            return "/list/%s?dir=%s" % (peid, fname)
        elif stat.S_ISREG(fstat.st_mode):
            return "/cat/%s" % fname
        else:
            return fname

    def getMtime(self, fstat):
        return time.ctime(fstat.st_mtime)

    def getDownload(self, fstat, fname):
        if stat.S_ISREG(fstat.st_mode):
            return '<a href="/down?dir=%s">Download</a>' % (fname)
        else:
            return ''

    def getSize(self, stats):
        return int(math.ceil(stats.st_size / 1024.0))
