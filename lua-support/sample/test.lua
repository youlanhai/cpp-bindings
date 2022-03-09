print("this message from lua.")

local myns = myns

local function main()
	local myClass = myns.MyClass()
	myClass.id = 1
	print("myClass.id", myClass.id)
	myClass = nil

	collectgarbage()

	local c = myns.SafeClass()
	c.onEnter = function(...)
		print("lua: onEnter:", ...)
	end
	c.onExit = function(...)
		print("lua: onExit:", ...)
	end

	c:enter()
	c:exit()

	c.id = 123
	c.name = "test"
	print("c.id:", c.id)
	print("c.name:", c.name)
end

main()
