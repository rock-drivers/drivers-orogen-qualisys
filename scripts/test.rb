#! /usr/bin/env ruby

require 'orocos'
include Orocos

if !ARGV[0]
    STDERR.puts "usage: test.rb host body_label"
    exit 1
end

ENV['PKG_CONFIG_PATH'] = "#{File.expand_path("..", File.dirname(__FILE__))}/build:#{ENV['PKG_CONFIG_PATH']}"

Orocos.initialize

Orocos::Process.run 'test_qualisys' do |p|
    driver = p.task 'Task'
    Orocos.log_all_ports

    driver.qtm_host = ARGV[0]
    driver.body_label = ARGV[1]

    driver.configure
    driver.start

    reader = driver.pose_samples.reader

    loop do
	if sample = reader.read
	    puts "%s %s %s" % [sample.position.data[0], sample.position.data[1], sample.position.data[2]]
	end
	sleep 0.01
    end

    driver.stop
end

