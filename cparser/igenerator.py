# -*- coding: utf-8 -*-
import os
from os import path
import export_config

class IGenerator(object):

	def __init__(self, generator_config):
		super(IGenerator, self).__init__()

		self.name_filter = generator_config["name_filter"]

		self.code_generators = {}
		self.all_types = {}

		self.generator_config = generator_config
		self.output_path = generator_config["output_path"]
		self.template_path = generator_config["template_path"]

		self.prefix = export_config.prefix
		self.abstract_classes = export_config.abstract_classes
		self.include_headers = export_config.include_headers

		self.templates = {}

	def load_template(self, name):
		text = self.templates.get(name)
		if text:
			return text

		file_path = path.join(self.template_path, name)
		with open(file_path, "r") as f:
			text = f.read()

		self.templates[name] = text
		return text

	def generate(self, type_info):
		generator = self.get_code_generator(type_info)
		return generator and generator.generate(type_info)

	def get_code_generator(self, type_info):
		tp = type(type_info)
		generator = self.code_generators.get(tp)
		if generator is None:
			print("failed find generator for type: ", tp)
		return generator


class IProcessor(object):

	def __init__(self, generator):
		super(IProcessor, self).__init__()

		self.generator = generator

	def generate(self, type_info):
		self.type_info = type_info
