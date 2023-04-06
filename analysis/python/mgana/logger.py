import logging

logging.basicConfig(format='%(levelname)s|%(filename)s (L-%(lineno)d) - %(message)s', level=logging.WARNING)
rootLogger = logging.getLogger('mgana')

# # Create handlers
# c_handler = logging.StreamHandler()
# c_handler.setLevel(logging.WARNING)

# # Create formatters and add it to handlers
# c_format = logging.Formatter('%(name)s - %(levelname)s - %(message)s')
# c_handler.setFormatter(c_format)

# logger.addHandler(c_handler)


# import logging.config

# def singleton(cls):
#     instances = {}
#     def get_instance():
#         if cls not in instances:
#             instances[cls] = cls()
#         return instances[cls]
#     return get_instance()

# @singleton
# class Logger():
#     def __init__(self):
#         logging.config.fileConfig('logging.conf')
#         self.logr = logging.getLogger('root')

