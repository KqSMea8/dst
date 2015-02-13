import os

import tornado.ioloop
import tornado.web
from tornado.options import define, options

from handler import root
from handler import list
from handler import cat
from handler import down

define("root", default = "/home/disk8/z/cluster/155", help="dstream deloy path")
define("port", default = "8888", help="file server bind port")

application = tornado.web.Application([
    (r"/", root.RootHandler),
    (r"/list/([0-9]+)", list.ListHandler),
    (r"/cat/([a-zA-Z0-9\.\-_/]+)", cat.CatHandler),
    (r"/down", down.DownHandler)
])

if __name__ == "__main__":
    tornado.options.parse_command_line()
    os.chdir(options.root)
    application.listen(int(options.port))
    tornado.ioloop.IOLoop.instance().start()
