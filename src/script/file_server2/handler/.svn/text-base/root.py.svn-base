import os, re
import tornado.web

class RootHandler(tornado.web.RequestHandler):
    def get(self):
        try:
            mfiles = [f for f in os.listdir('.') if re.search('v[0-9]+_([0-9]+)', f)]
            peids = map(lambda x: re.search('_([0-9]+)', x).group(0).strip('_'), mfiles)
            self.write('<h2>List All Available PEs</h2>')
            self.write('<ul>')
            for pe in sorted(set(peids), cmp = lambda x, y: int(x) - int(y), reverse = True):
                self.write('<li><a href="/list/%s">%s</a></li>' % (pe, pe))
            self.write('</ul>')
        except:
            raise tornado.web.HTTPError(403)
