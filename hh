-- XuilanLib.lua (FULL, copy-paste)
-- No junk at end.

local Players = game:GetService("Players")
local UserInputService = game:GetService("UserInputService")
local RunService = game:GetService("RunService")
local TweenService = game:GetService("TweenService")
local TextService = game:GetService("TextService")

local XuilanLib = {}
XuilanLib.__index = XuilanLib

--==================================================
-- Helpers
--==================================================
local function ParseFont(v)
	if typeof(v) == "EnumItem" then return v end
	if typeof(v) == "string" and Enum.Font[v] then
		return Enum.Font[v]
	end
	return Enum.Font.SourceSans
end

local function ParseColor(v)
	if typeof(v) == "Color3" then return v end
	if typeof(v) == "table" then
		return Color3.fromRGB(v[1] or 255, v[2] or 255, v[3] or 255)
	end
	if typeof(v) == "string" then
		local hex = v:gsub("#","")
		if #hex == 6 then
			local r = tonumber(hex:sub(1,2),16)
			local g = tonumber(hex:sub(3,4),16)
			local b = tonumber(hex:sub(5,6),16)
			if r and g and b then return Color3.fromRGB(r,g,b) end
		end
		local r,g,b = v:match("(%d+)%s*,%s*(%d+)%s*,%s*(%d+)")
		if r and g and b then return Color3.fromRGB(tonumber(r),tonumber(g),tonumber(b)) end
	end
	return Color3.fromRGB(255,255,255)
end

local function asAsset(v)
	if not v then return nil end
	if typeof(v) == "number" then return "rbxassetid://"..tostring(v) end
	if typeof(v) == "string" then
		if v:find("rbxassetid://") then return v end
		if v:match("^%d+$") then return "rbxassetid://"..v end
		return v
	end
	return nil
end

local function clampTextSize(n, fallback)
	n = tonumber(n)
	if not n then return fallback end
	n = math.floor(n + 0.5)
	if n < 6 then n = 6 end
	if n > 44 then n = 44 end
	return n
end

local function brighten(c, add)
	return Color3.fromRGB(
		math.clamp(c.R*255 + add, 0, 255),
		math.clamp(c.G*255 + add, 0, 255),
		math.clamp(c.B*255 + add, 0, 255)
	)
end

local function tableShallowCopy(t)
	local n = {}
	for k,v in pairs(t) do n[k]=v end
	return n
end

--==================================================
-- Themes (15)
--==================================================
local Themes = {
	{ Name="Default Dark", Main=Color3.fromRGB(18,18,18), Inner=Color3.fromRGB(115,115,115), Row=Color3.fromRGB(150,150,150), ValueBox=Color3.fromRGB(115,115,115), PillOff=Color3.fromRGB(145,145,145), Fill=Color3.fromRGB(100,100,100), Lines=Color3.fromRGB(120,120,120), Accent=Color3.fromRGB(39,227,36), Title=Color3.fromRGB(235,235,235), Desc=Color3.fromRGB(200,200,200), Text=Color3.fromRGB(255,255,255), Muted=Color3.fromRGB(220,220,220) },
	{ Name="Midnight",     Main=Color3.fromRGB(10,10,16), Inner=Color3.fromRGB(105,105,130), Row=Color3.fromRGB(140,140,165), ValueBox=Color3.fromRGB(105,105,125), PillOff=Color3.fromRGB(125,125,145), Fill=Color3.fromRGB(90,90,110), Lines=Color3.fromRGB(140,140,165), Accent=Color3.fromRGB(39,227,36), Title=Color3.fromRGB(245,245,255), Desc=Color3.fromRGB(205,205,220), Text=Color3.fromRGB(255,255,255), Muted=Color3.fromRGB(230,230,240) },
	{ Name="Obsidian",     Main=Color3.fromRGB(12,12,12), Inner=Color3.fromRGB(110,110,110), Row=Color3.fromRGB(145,145,145), ValueBox=Color3.fromRGB(110,110,110), PillOff=Color3.fromRGB(135,135,135), Fill=Color3.fromRGB(95,95,95), Lines=Color3.fromRGB(135,135,135), Accent=Color3.fromRGB(39,227,36), Title=Color3.fromRGB(245,245,245), Desc=Color3.fromRGB(210,210,210), Text=Color3.fromRGB(255,255,255), Muted=Color3.fromRGB(235,235,235) },
	{ Name="Graphite",     Main=Color3.fromRGB(22,22,24), Inner=Color3.fromRGB(130,130,140), Row=Color3.fromRGB(170,170,180), ValueBox=Color3.fromRGB(135,135,145), PillOff=Color3.fromRGB(160,160,170), Fill=Color3.fromRGB(120,120,130), Lines=Color3.fromRGB(150,150,160), Accent=Color3.fromRGB(39,227,36), Title=Color3.fromRGB(250,250,250), Desc=Color3.fromRGB(215,215,215), Text=Color3.fromRGB(255,255,255), Muted=Color3.fromRGB(235,235,235) },
	{ Name="Forest",       Main=Color3.fromRGB(12,18,14), Inner=Color3.fromRGB(115,135,120), Row=Color3.fromRGB(150,170,155), ValueBox=Color3.fromRGB(120,140,125), PillOff=Color3.fromRGB(135,155,140), Fill=Color3.fromRGB(95,115,100), Lines=Color3.fromRGB(150,170,155), Accent=Color3.fromRGB(39,227,36), Title=Color3.fromRGB(245,255,245), Desc=Color3.fromRGB(210,220,210), Text=Color3.fromRGB(255,255,255), Muted=Color3.fromRGB(235,245,235) },
	{ Name="Aqua",         Main=Color3.fromRGB(10,16,18), Inner=Color3.fromRGB(115,140,140), Row=Color3.fromRGB(150,175,175), ValueBox=Color3.fromRGB(120,145,145), PillOff=Color3.fromRGB(135,160,160), Fill=Color3.fromRGB(95,120,120), Lines=Color3.fromRGB(150,175,175), Accent=Color3.fromRGB(39,227,36), Title=Color3.fromRGB(245,255,255), Desc=Color3.fromRGB(210,220,220), Text=Color3.fromRGB(255,255,255), Muted=Color3.fromRGB(235,245,245) },
	{ Name="Rose",         Main=Color3.fromRGB(18,10,12), Inner=Color3.fromRGB(140,115,125), Row=Color3.fromRGB(175,150,160), ValueBox=Color3.fromRGB(140,115,125), PillOff=Color3.fromRGB(160,140,150), Fill=Color3.fromRGB(120,95,105), Lines=Color3.fromRGB(175,150,160), Accent=Color3.fromRGB(39,227,36), Title=Color3.fromRGB(255,245,250), Desc=Color3.fromRGB(220,210,215), Text=Color3.fromRGB(255,255,255), Muted=Color3.fromRGB(245,235,240) },
	{ Name="Violet",       Main=Color3.fromRGB(16,12,22), Inner=Color3.fromRGB(130,120,160), Row=Color3.fromRGB(165,150,190), ValueBox=Color3.fromRGB(130,120,160), PillOff=Color3.fromRGB(150,140,175), Fill=Color3.fromRGB(110,95,135), Lines=Color3.fromRGB(165,150,190), Accent=Color3.fromRGB(39,227,36), Title=Color3.fromRGB(250,245,255), Desc=Color3.fromRGB(215,210,220), Text=Color3.fromRGB(255,255,255), Muted=Color3.fromRGB(240,235,245) },
	{ Name="Crimson",      Main=Color3.fromRGB(18,10,10), Inner=Color3.fromRGB(145,115,115), Row=Color3.fromRGB(180,150,150), ValueBox=Color3.fromRGB(145,115,115), PillOff=Color3.fromRGB(165,140,140), Fill=Color3.fromRGB(120,95,95), Lines=Color3.fromRGB(180,150,150), Accent=Color3.fromRGB(39,227,36), Title=Color3.fromRGB(255,245,245), Desc=Color3.fromRGB(220,210,210), Text=Color3.fromRGB(255,255,255), Muted=Color3.fromRGB(245,235,235) },
	{ Name="Steel",        Main=Color3.fromRGB(14,16,18), Inner=Color3.fromRGB(125,135,145), Row=Color3.fromRGB(160,170,180), ValueBox=Color3.fromRGB(125,135,145), PillOff=Color3.fromRGB(150,160,170), Fill=Color3.fromRGB(105,115,125), Lines=Color3.fromRGB(160,170,180), Accent=Color3.fromRGB(39,227,36), Title=Color3.fromRGB(245,250,255), Desc=Color3.fromRGB(210,215,220), Text=Color3.fromRGB(255,255,255), Muted=Color3.fromRGB(235,240,245) },
	{ Name="Sand",         Main=Color3.fromRGB(18,16,12), Inner=Color3.fromRGB(145,140,120), Row=Color3.fromRGB(175,170,150), ValueBox=Color3.fromRGB(145,140,120), PillOff=Color3.fromRGB(160,155,135), Fill=Color3.fromRGB(120,115,95), Lines=Color3.fromRGB(175,170,150), Accent=Color3.fromRGB(39,227,36), Title=Color3.fromRGB(255,255,245), Desc=Color3.fromRGB(220,220,210), Text=Color3.fromRGB(255,255,255), Muted=Color3.fromRGB(245,245,235) },
	{ Name="Neon Dark",    Main=Color3.fromRGB(8,8,10), Inner=Color3.fromRGB(105,105,120), Row=Color3.fromRGB(140,140,160), ValueBox=Color3.fromRGB(105,105,120), PillOff=Color3.fromRGB(125,125,145), Fill=Color3.fromRGB(90,90,110), Lines=Color3.fromRGB(150,150,170), Accent=Color3.fromRGB(39,227,36), Title=Color3.fromRGB(255,255,255), Desc=Color3.fromRGB(215,215,225), Text=Color3.fromRGB(255,255,255), Muted=Color3.fromRGB(230,230,240) },
	{ Name="Deep Blue",    Main=Color3.fromRGB(10,12,20), Inner=Color3.fromRGB(115,125,155), Row=Color3.fromRGB(145,155,185), ValueBox=Color3.fromRGB(115,125,155), PillOff=Color3.fromRGB(135,145,175), Fill=Color3.fromRGB(95,105,135), Lines=Color3.fromRGB(145,155,185), Accent=Color3.fromRGB(39,227,36), Title=Color3.fromRGB(245,250,255), Desc=Color3.fromRGB(210,215,225), Text=Color3.fromRGB(255,255,255), Muted=Color3.fromRGB(235,240,245) },
	{ Name="Olive",        Main=Color3.fromRGB(14,16,10), Inner=Color3.fromRGB(130,135,115), Row=Color3.fromRGB(165,170,145), ValueBox=Color3.fromRGB(130,135,115), PillOff=Color3.fromRGB(150,155,135), Fill=Color3.fromRGB(105,110,95), Lines=Color3.fromRGB(165,170,145), Accent=Color3.fromRGB(39,227,36), Title=Color3.fromRGB(250,255,245), Desc=Color3.fromRGB(215,220,210), Text=Color3.fromRGB(255,255,255), Muted=Color3.fromRGB(240,245,235) },
	{ Name="Warm Grey",    Main=Color3.fromRGB(18,16,16), Inner=Color3.fromRGB(135,130,130), Row=Color3.fromRGB(170,165,165), ValueBox=Color3.fromRGB(135,130,130), PillOff=Color3.fromRGB(155,150,150), Fill=Color3.fromRGB(115,110,110), Lines=Color3.fromRGB(170,165,165), Accent=Color3.fromRGB(39,227,36), Title=Color3.fromRGB(250,250,250), Desc=Color3.fromRGB(215,215,215), Text=Color3.fromRGB(255,255,255), Muted=Color3.fromRGB(240,240,240) },
}

local function getThemeByName(name)
	for _,t in ipairs(Themes) do
		if t.Name == name then return t end
	end
	return Themes[1]
end

--==================================================
-- Notifications (width 213)
--==================================================
local function CreateNotifier(screenGui, getTheme, MAIN_T, MAIN_CORNER)
	local holder = Instance.new("Frame")
	holder.Name = "NotificationsHolder"
	holder.BackgroundTransparency = 1
	holder.BorderSizePixel = 0
	holder.AnchorPoint = Vector2.new(1,1)
	holder.Position = UDim2.new(1,-12, 1,-12)
	holder.Size = UDim2.new(0, 213, 0, 200) -- 1/3 narrower
	holder.ZIndex = 9000
	holder.Parent = screenGui

	local list = Instance.new("UIListLayout")
	list.SortOrder = Enum.SortOrder.LayoutOrder
	list.FillDirection = Enum.FillDirection.Vertical
	list.VerticalAlignment = Enum.VerticalAlignment.Bottom
	list.Padding = UDim.new(0,8)
	list.Parent = holder

	local Notifier = {}

	function Notifier:Notify(opt)
		opt = opt or {}
		local theme = getTheme()
		local titleText = tostring(opt.Title or "Notification")
		local descText = tostring(opt.Description or "")
		local duration = tonumber(opt.Duration) or 2.0

		local wrap = Instance.new("Frame")
		wrap.BackgroundTransparency = 1
		wrap.BorderSizePixel = 0
		wrap.Size = UDim2.new(1,0,0,64)
		wrap.LayoutOrder = math.floor(os.clock()*1000)
		wrap.Parent = holder

		local card = Instance.new("Frame")
		card.BorderSizePixel = 0
		card.BackgroundColor3 = theme.Main
		card.BackgroundTransparency = MAIN_T
		card.Size = UDim2.new(1,0,1,0)
		card.Position = UDim2.new(1, 260, 0, 0)
		card.ZIndex = 9001
		card.Parent = wrap

		local cr = Instance.new("UICorner")
		cr.CornerRadius = UDim.new(0, MAIN_CORNER)
		cr.Parent = card

		local pad = Instance.new("UIPadding")
		pad.PaddingLeft = UDim.new(0, 12)
		pad.PaddingRight = UDim.new(0, 12)
		pad.PaddingTop = UDim.new(0, 10)
		pad.PaddingBottom = UDim.new(0, 10)
		pad.Parent = card

		local tl = Instance.new("TextLabel")
		tl.BackgroundTransparency = 1
		tl.BorderSizePixel = 0
		tl.Text = titleText
		tl.Font = Enum.Font.SourceSansBold
		tl.TextSize = 18
		tl.TextColor3 = theme.Title
		tl.TextXAlignment = Enum.TextXAlignment.Left
		tl.TextYAlignment = Enum.TextYAlignment.Top
		tl.Size = UDim2.new(1,0,0,20)
		tl.ZIndex = 9002
		tl.Parent = card

		local dl = Instance.new("TextLabel")
		dl.BackgroundTransparency = 1
		dl.BorderSizePixel = 0
		dl.Text = descText
		dl.Font = Enum.Font.SourceSans
		dl.TextSize = 14
		dl.TextColor3 = theme.Desc
		dl.TextXAlignment = Enum.TextXAlignment.Left
		dl.TextYAlignment = Enum.TextYAlignment.Top
		dl.TextWrapped = true
		dl.Position = UDim2.new(0,0,0,22)
		dl.Size = UDim2.new(1,0,1,-22)
		dl.ZIndex = 9002
		dl.Parent = card

		TweenService:Create(card, TweenInfo.new(0.3, Enum.EasingStyle.Quad, Enum.EasingDirection.Out), {
			Position = UDim2.new(0,0,0,0)
		}):Play()

		task.delay(duration, function()
			if not card or not card.Parent then return end
			local t = TweenService:Create(card, TweenInfo.new(0.3, Enum.EasingStyle.Quad, Enum.EasingDirection.In), {
				Position = UDim2.new(1, 260, 0, 0)
			})
			t:Play()
			t.Completed:Connect(function()
				if wrap and wrap.Parent then wrap:Destroy() end
			end)
		end)
	end

	return Notifier
end

local function ShouldNotify(mode, state)
	mode = tostring(mode or "Both")
	if mode == "Both" then return true end
	if mode == "On" then return state == true end
	if mode == "Off" then return state == false end
	return false
end

--==================================================
-- CreateWindow
--==================================================
function XuilanLib:CreateWindow(settings)
	settings = settings or {}

	local player = Players.LocalPlayer
	local playerGui = player:WaitForChild("PlayerGui")

	-- remove old
	local old = playerGui:FindFirstChild("CustomRectangleGui")
	if old then old:Destroy() end

	-- STYLE (as you already had)
	local MAIN_SIZE = UDim2.new(0,400,0,280)
	local MAIN_T = 0.12
	local MAIN_CORNER = 14

	local INNER_SIZE = UDim2.new(0,260,0,235)
	local INNER_T = 0.8
	local INNER_CORNER = 14

	local ROW_CORNER = 12
	local ROW_BG_T = INNER_T
	local ROW_H = 34

	local HEADER_TOP_PAD = 3
	local HEADER_BOTTOM_PAD = 3
	local LEFT_TO_IMAGE = 10
	local IMAGE_TO_TEXT = 10
	local ICON_MARGIN = 10
	local ICON_GAP = 10

	-- assets (your latest)
	local ASSET_PUG = asAsset(settings.Icon) or "rbxassetid://136007050760089"
	local ASSET_CLOSE = "rbxassetid://87923978940368"
	local ASSET_MINUS = "rbxassetid://92670491990824"
	local ASSET_FINGERPRINT = "rbxassetid://134643086907470"
	local ASSET_ARROWS = "rbxassetid://97961707453604"

	-- header text settings
	local windowTitle = settings.Title or "XuilanLib"
	local windowDesc = settings.Description or ""
	local titleFont = ParseFont(settings.TitleFont or "SourceSansBold")
	local descFont = ParseFont(settings.DescriptionFont or "SourceSansBold")
	local titleColor = ParseColor(settings.TitleColor or "#ebebeb")
	local descColor = ParseColor(settings.DescriptionColor or "200,200,200")

	-- current theme
	local currentTheme = Themes[1]

	-- ScreenGui
	local screenGui = Instance.new("ScreenGui")
	screenGui.Name = "CustomRectangleGui"
	screenGui.ResetOnSpawn = false
	screenGui.ZIndexBehavior = Enum.ZIndexBehavior.Sibling
	screenGui.DisplayOrder = 999999
	screenGui.Parent = playerGui

	-- notifier
	local Notifier = CreateNotifier(screenGui, function() return currentTheme end, MAIN_T, MAIN_CORNER)

	-- MAIN
	local mainFrame = Instance.new("Frame")
	mainFrame.Name = "MainRectangle"
	mainFrame.Size = MAIN_SIZE
	mainFrame.AnchorPoint = Vector2.new(0.5,0.5)
	mainFrame.Position = UDim2.new(0.5,0,0.5,0)
	mainFrame.BackgroundColor3 = currentTheme.Main
	mainFrame.BackgroundTransparency = MAIN_T
	mainFrame.BorderSizePixel = 0
	mainFrame.Active = true
	mainFrame.ZIndex = 1000
	mainFrame.Parent = screenGui

	local mainCorner = Instance.new("UICorner")
	mainCorner.CornerRadius = UDim.new(0, MAIN_CORNER)
	mainCorner.Parent = mainFrame

	-- INNER
	local innerFrame = Instance.new("Frame")
	innerFrame.Name = "InnerRectangle"
	innerFrame.Size = INNER_SIZE
	innerFrame.AnchorPoint = Vector2.new(1,1)
	innerFrame.Position = UDim2.new(1,-10, 1,-10)
	innerFrame.BackgroundColor3 = currentTheme.Inner
	innerFrame.BackgroundTransparency = INNER_T
	innerFrame.BorderSizePixel = 0
	innerFrame.ZIndex = 1001
	innerFrame.Parent = mainFrame

	local innerCorner = Instance.new("UICorner")
	innerCorner.CornerRadius = UDim.new(0, INNER_CORNER)
	innerCorner.Parent = innerFrame

	-- mask to prevent overflow (fix your bug)
	local innerMask = Instance.new("Frame")
	innerMask.Name = "InnerMask"
	innerMask.BackgroundTransparency = 1
	innerMask.BorderSizePixel = 0
	innerMask.ClipsDescendants = true
	innerMask.Size = UDim2.new(1,0,1,0)
	innerMask.ZIndex = 1002
	innerMask.Parent = innerFrame

	local innerMaskCorner = Instance.new("UICorner")
	innerMaskCorner.CornerRadius = UDim.new(0, INNER_CORNER)
	innerMaskCorner.Parent = innerMask

	-- Header image
	local image = Instance.new("ImageLabel")
	image.Name = "TopLeftImage"
	image.BackgroundTransparency = 1
	image.BorderSizePixel = 0
	image.Image = ASSET_PUG
	image.ScaleType = Enum.ScaleType.Fit
	image.ZIndex = 1002
	image.Parent = mainFrame

	-- Header text wrap
	local textWrap = Instance.new("Frame")
	textWrap.BackgroundTransparency = 1
	textWrap.BorderSizePixel = 0
	textWrap.ZIndex = 1002
	textWrap.Parent = mainFrame

	local headerList = Instance.new("UIListLayout")
	headerList.FillDirection = Enum.FillDirection.Vertical
	headerList.SortOrder = Enum.SortOrder.LayoutOrder
	headerList.Parent = textWrap

	local title = Instance.new("TextLabel")
	title.BackgroundTransparency = 1
	title.BorderSizePixel = 0
	title.Text = windowTitle
	title.Font = titleFont
	title.TextColor3 = titleColor
	title.TextScaled = true
	title.TextXAlignment = Enum.TextXAlignment.Left
	title.TextYAlignment = Enum.TextYAlignment.Top
	title.ZIndex = 1003
	title.LayoutOrder = 1
	title.Parent = textWrap

	local desc = Instance.new("TextLabel")
	desc.BackgroundTransparency = 1
	desc.BorderSizePixel = 0
	desc.Text = windowDesc
	desc.Font = descFont
	desc.TextColor3 = descColor
	desc.TextScaled = true
	desc.TextXAlignment = Enum.TextXAlignment.Left
	desc.TextYAlignment = Enum.TextYAlignment.Top
	desc.ZIndex = 1003
	desc.LayoutOrder = 2
	desc.Parent = textWrap

	-- Close & Minus
	local closeBtn = Instance.new("ImageButton")
	closeBtn.BackgroundTransparency = 1
	closeBtn.BorderSizePixel = 0
	closeBtn.AutoButtonColor = false
	closeBtn.AnchorPoint = Vector2.new(1,0)
	closeBtn.Image = ASSET_CLOSE
	closeBtn.ScaleType = Enum.ScaleType.Fit
	closeBtn.ZIndex = 2000
	closeBtn.Parent = mainFrame

	local minusBtn = Instance.new("ImageButton")
	minusBtn.BackgroundTransparency = 1
	minusBtn.BorderSizePixel = 0
	minusBtn.AutoButtonColor = false
	minusBtn.AnchorPoint = Vector2.new(1,0)
	minusBtn.Image = ASSET_MINUS
	minusBtn.ScaleType = Enum.ScaleType.Fit
	minusBtn.ZIndex = 2000
	minusBtn.Parent = mainFrame

	local function press1px(btn)
		local p = btn.Position
		local down = UDim2.new(p.X.Scale, p.X.Offset+1, p.Y.Scale, p.Y.Offset+1)
		local t1 = TweenService:Create(btn, TweenInfo.new(0.05, Enum.EasingStyle.Quad, Enum.EasingDirection.Out), {Position=down})
		local t2 = TweenService:Create(btn, TweenInfo.new(0.05, Enum.EasingStyle.Quad, Enum.EasingDirection.Out), {Position=p})
		t1:Play()
		t1.Completed:Connect(function() t2:Play() end)
	end

	closeBtn.Activated:Connect(function()
		press1px(closeBtn)
		task.delay(0.1, function()
			if screenGui then screenGui:Destroy() end
		end)
	end)

	--==================================================
	-- Open Ui pill (FIXED EXACTLY as you demanded)
	--==================================================
	local openPill = Instance.new("Frame")
	openPill.Name = "OpenPill"
	openPill.Visible = false
	openPill.BorderSizePixel = 0
	openPill.AnchorPoint = Vector2.new(0.5,0)
	openPill.Position = UDim2.new(0.5,0,0,12)
	openPill.Size = UDim2.new(0,140,0,34) -- auto
	openPill.BackgroundColor3 = currentTheme.Main
	openPill.BackgroundTransparency = MAIN_T
	openPill.ZIndex = 3000
	openPill.Parent = screenGui

	local openPillCorner = Instance.new("UICorner")
	openPillCorner.CornerRadius = UDim.new(1,0)
	openPillCorner.Parent = openPill

	local arrowsSize = math.floor(16*1.3 + 0.5)

	local arrowsBtn = Instance.new("ImageButton")
	arrowsBtn.Name = "DragArrows"
	arrowsBtn.BackgroundTransparency = 1
	arrowsBtn.BorderSizePixel = 0
	arrowsBtn.AutoButtonColor = false
	arrowsBtn.Image = ASSET_ARROWS
	arrowsBtn.ScaleType = Enum.ScaleType.Fit
	arrowsBtn.Size = UDim2.new(0,arrowsSize,0,arrowsSize)
	arrowsBtn.Position = UDim2.new(0,8, 0.5, -math.floor(arrowsSize/2))
	arrowsBtn.ZIndex = 3001
	arrowsBtn.Parent = openPill

	-- open ONLY by this button, but we block clicks on arrows area
	local openBtn = Instance.new("TextButton")
	openBtn.Name = "OpenButton"
	openBtn.BackgroundTransparency = 1
	openBtn.BorderSizePixel = 0
	openBtn.AutoButtonColor = false
	openBtn.Text = ""
	openBtn.Size = UDim2.new(1,0,1,0)
	openBtn.ZIndex = 3001
	openBtn.Parent = openPill

	local openText = Instance.new("TextLabel")
	openText.BackgroundTransparency = 1
	openText.BorderSizePixel = 0
	openText.Font = Enum.Font.SourceSansBold
	openText.Text = "Open Ui"
	openText.TextColor3 = Color3.fromRGB(255,255,255)
	openText.TextSize = 16
	openText.TextXAlignment = Enum.TextXAlignment.Left
	openText.TextYAlignment = Enum.TextYAlignment.Center
	openText.Position = UDim2.new(0, 8 + arrowsSize + 8 + 5, 0, -2) -- +5 right, -2 up
	openText.Size = UDim2.new(1, - (8 + arrowsSize + 8 + 5 + 8), 1, 0)
	openText.ZIndex = 3002
	openText.Parent = openPill

	local function updateOpenPillWidth()
		local bounds = TextService:GetTextSize(openText.Text, openText.TextSize, openText.Font, Vector2.new(1000,1000))
		local w = 8 + arrowsSize + 8 + 5 + bounds.X + 8
		if w < 140 then w = 140 end
		openPill.Size = UDim2.new(0, math.floor(w+0.5), 0, 34)
	end
	updateOpenPillWidth()
	openText:GetPropertyChangedSignal("Text"):Connect(updateOpenPillWidth)
	openText:GetPropertyChangedSignal("TextSize"):Connect(updateOpenPillWidth)
	openText:GetPropertyChangedSignal("Font"):Connect(updateOpenPillWidth)

	openBtn.Activated:Connect(function()
		-- block if click inside arrows area
		local m = UserInputService:GetMouseLocation()
		local aPos, aSize = arrowsBtn.AbsolutePosition, arrowsBtn.AbsoluteSize
		local inside = (m.X >= aPos.X and m.X <= aPos.X+aSize.X and m.Y >= aPos.Y and m.Y <= aPos.Y+aSize.Y)
		if inside then return end

		openPill.Visible = false
		mainFrame.Visible = true
	end)

	-- drag ONLY by arrows
	local pillDragging = false
	local pillActiveInput = nil
	local pillDragStart, pillStartPos

	arrowsBtn.InputBegan:Connect(function(input)
		if input.UserInputType == Enum.UserInputType.MouseButton1 or input.UserInputType == Enum.UserInputType.Touch then
			pillDragging = true
			pillActiveInput = input
			pillDragStart = input.Position
			pillStartPos = openPill.Position

			input.Changed:Connect(function()
				if input.UserInputState == Enum.UserInputState.End then
					if pillActiveInput == input then
						pillDragging = false
						pillActiveInput = nil
					end
				end
			end)
		end
	end)

	UserInputService.InputChanged:Connect(function(input)
		if pillDragging and pillActiveInput and input == pillActiveInput and pillDragStart and pillStartPos then
			local delta = input.Position - pillDragStart
			openPill.Position = UDim2.new(
				pillStartPos.X.Scale, pillStartPos.X.Offset + delta.X,
				pillStartPos.Y.Scale, pillStartPos.Y.Offset + delta.Y
			)
		end
	end)

	minusBtn.Activated:Connect(function()
		press1px(minusBtn)
		task.delay(0.1, function()
			mainFrame.Visible = false
			openPill.Visible = true
		end)
	end)

	--==================================================
	-- Main drag (fixed)
	--==================================================
	local draggingMain = false
	local activeMainInput = nil
	local mainDragStart, mainStartPos

	mainFrame.InputBegan:Connect(function(input)
		if input.UserInputType == Enum.UserInputType.MouseButton1 or input.UserInputType == Enum.UserInputType.Touch then
			draggingMain = true
			activeMainInput = input
			mainDragStart = input.Position
			mainStartPos = mainFrame.Position

			input.Changed:Connect(function()
				if input.UserInputState == Enum.UserInputState.End then
					if activeMainInput == input then
						draggingMain = false
						activeMainInput = nil
					end
				end
			end)
		end
	end)

	UserInputService.InputChanged:Connect(function(input)
		if draggingMain and activeMainInput and input == activeMainInput and mainDragStart and mainStartPos then
			local delta = input.Position - mainDragStart
			mainFrame.Position = UDim2.new(
				mainStartPos.X.Scale, mainStartPos.X.Offset + delta.X,
				mainStartPos.Y.Scale, mainStartPos.Y.Offset + delta.Y
			)
		end
	end)

	--==================================================
	-- Sections panel (left) + content (right)
	--==================================================
	local sectionsPanel = Instance.new("ScrollingFrame")
	sectionsPanel.BackgroundTransparency = 1
	sectionsPanel.BorderSizePixel = 0
	sectionsPanel.ScrollBarThickness = 0
	sectionsPanel.ScrollingDirection = Enum.ScrollingDirection.Y
	sectionsPanel.AutomaticCanvasSize = Enum.AutomaticSize.Y
	sectionsPanel.CanvasSize = UDim2.new(0,0,0,0)
	sectionsPanel.ZIndex = 1002
	sectionsPanel.Parent = mainFrame

	local sectionsList = Instance.new("UIListLayout")
	sectionsList.SortOrder = Enum.SortOrder.LayoutOrder
	sectionsList.Padding = UDim.new(0,2) -- closer
	sectionsList.Parent = sectionsPanel

	local contentScroll = Instance.new("ScrollingFrame")
	contentScroll.BackgroundTransparency = 1
	contentScroll.BorderSizePixel = 0
	contentScroll.ScrollBarThickness = 0
	contentScroll.ScrollingDirection = Enum.ScrollingDirection.Y
	contentScroll.ElasticBehavior = Enum.ElasticBehavior.WhenScrollable
	contentScroll.AutomaticCanvasSize = Enum.AutomaticSize.Y
	contentScroll.CanvasSize = UDim2.new(0,0,0,0)
	contentScroll.ZIndex = 1010
	contentScroll.Parent = innerMask

	local contentPad = Instance.new("UIPadding")
	contentPad.PaddingLeft = UDim.new(0,10)
	contentPad.PaddingRight = UDim.new(0,10)
	contentPad.PaddingTop = UDim.new(0,10)
	contentPad.PaddingBottom = UDim.new(0,10)
	contentPad.Parent = contentScroll

	local contentList = Instance.new("UIListLayout")
	contentList.SortOrder = Enum.SortOrder.LayoutOrder
	contentList.Padding = UDim.new(0,8)
	contentList.Parent = contentScroll

	-- bottom safe spacer (helps avoid rounding overlap)
	local bottomSpacer = Instance.new("Frame")
	bottomSpacer.BackgroundTransparency = 1
	bottomSpacer.BorderSizePixel = 0
	bottomSpacer.Size = UDim2.new(1,0,0,INNER_CORNER)
	bottomSpacer.LayoutOrder = 999999
	bottomSpacer.Parent = contentScroll

	--==================================================
	-- Dropdown global close
	--==================================================
	local openDropdownMenu = nil
	local function closeDropdownMenu()
		if openDropdownMenu and openDropdownMenu.Parent then
			openDropdownMenu:Destroy()
		end
		openDropdownMenu = nil
	end

	UserInputService.InputBegan:Connect(function(input, gp)
		if gp then return end
		if input.UserInputType == Enum.UserInputType.MouseButton1 or input.UserInputType == Enum.UserInputType.Touch then
			if openDropdownMenu then closeDropdownMenu() end
		end
	end)

	--==================================================
	-- Window object + theme targets
	--==================================================
	local Window = {}
	Window.Sections = {}
	Window._sectionButtons = {}
	Window._selectedIndex = nil

	local ThemeTargets = {
		Main = mainFrame,
		Inner = innerFrame,
		OpenPill = openPill,
		Title = title,
		Desc = desc,
		SectionButtons = {},
		Rows = {},
	}

	local function ApplyTheme(theme)
		currentTheme = theme

		mainFrame.BackgroundColor3 = theme.Main
		mainFrame.BackgroundTransparency = MAIN_T

		innerFrame.BackgroundColor3 = theme.Inner
		innerFrame.BackgroundTransparency = INNER_T

		openPill.BackgroundColor3 = theme.Main
		openPill.BackgroundTransparency = MAIN_T

		title.TextColor3 = theme.Title
		desc.TextColor3 = theme.Desc

		for _, b in ipairs(ThemeTargets.SectionButtons) do
			b.TextColor3 = theme.Title
		end

		for _, r in ipairs(ThemeTargets.Rows) do
			if r.Content and r.Content.Parent then
				r.Content.BackgroundColor3 = theme.Row
				r.Content.BackgroundTransparency = ROW_BG_T
			end
			if r.Label and r.Label.Parent then
				r.Label.TextColor3 = theme.Text
			end
			if r.LabelMuted and r.LabelMuted.Parent then
				r.LabelMuted.TextColor3 = theme.Muted
			end
			if r.Type == "Toggle" then
				if r.Pill and r.Pill.Parent then
					r.Pill.BackgroundColor3 = (r.StateBox.Value and theme.Accent or theme.PillOff)
				end
			elseif r.Type == "Slider" then
				if r.Track and r.Track.Parent then r.Track.BackgroundColor3 = theme.PillOff end
				if r.Fill and r.Fill.Parent then r.Fill.BackgroundColor3 = theme.Fill end
				if r.ValueBox and r.ValueBox.Parent then r.ValueBox.BackgroundColor3 = theme.ValueBox end
			elseif r.Type == "Dropdown" then
				if r.SelectBtn and r.SelectBtn.Parent then r.SelectBtn.BackgroundColor3 = theme.ValueBox end
			elseif r.Type == "TextBox" then
				if r.InputBox and r.InputBox.Parent then r.InputBox.BackgroundColor3 = theme.ValueBox end
			elseif r.Type == "HeadText" then
				if r.LeftLine and r.LeftLine.Parent then r.LeftLine.BackgroundColor3 = theme.Lines end
				if r.RightLine and r.RightLine.Parent then r.RightLine.BackgroundColor3 = theme.Lines end
			end
		end
	end

	function Window:SetTheme(themeName)
		ApplyTheme(getThemeByName(themeName))
	end

	function Window:Notify(opt)
		Notifier:Notify(opt)
	end

	--==================================================
	-- Layout (header & panels)
	--==================================================
	local function updateLayout()
		local mainTop = mainFrame.AbsolutePosition.Y
		local innerTop = innerFrame.AbsolutePosition.Y

		local headerHeight = math.floor((innerTop - mainTop) - (HEADER_TOP_PAD + HEADER_BOTTOM_PAD))
		if headerHeight < 1 then headerHeight = 1 end

		local iconSize = math.max(12, math.floor(headerHeight * 0.60))
		closeBtn.Size = UDim2.new(0,iconSize,0,iconSize)
		closeBtn.Position = UDim2.new(1,-ICON_MARGIN,0,ICON_MARGIN)

		minusBtn.Size = UDim2.new(0,iconSize,0,iconSize)
		minusBtn.Position = UDim2.new(1, -(ICON_MARGIN + iconSize + ICON_GAP), 0, ICON_MARGIN)

		image.Position = UDim2.new(0,LEFT_TO_IMAGE,0,HEADER_TOP_PAD)
		image.Size = UDim2.new(0,headerHeight,0,headerHeight)

		local reservedRight = ICON_MARGIN + iconSize + ICON_GAP + iconSize + 12
		local availW = mainFrame.AbsoluteSize.X - (LEFT_TO_IMAGE + headerHeight + IMAGE_TO_TEXT) - reservedRight
		if availW < 10 then availW = 10 end

		textWrap.Position = UDim2.new(0, LEFT_TO_IMAGE + headerHeight + IMAGE_TO_TEXT, 0, HEADER_TOP_PAD)
		textWrap.Size = UDim2.new(0, availW, 0, headerHeight)

		local usableH = headerHeight - 4
		if usableH < 2 then usableH = 2 end
		local titleH = math.floor(usableH * 0.58)
		local descH = usableH - titleH
		title.Size = UDim2.new(1,0,0,titleH)
		desc.Size = UDim2.new(1,0,0,descH)

		local mainW = mainFrame.AbsoluteSize.X
		local mainH = mainFrame.AbsoluteSize.Y
		local innerW = innerFrame.AbsoluteSize.X
		local innerLeft = mainW - 10 - innerW

		local panelX = 3
		local panelRight = innerLeft - 3
		local panelW = panelRight - panelX
		if panelW < 20 then panelW = 20 end

		local panelY = math.floor(innerTop - mainTop)
		local panelH = mainH - panelY - 10
		if panelH < 20 then panelH = 20 end

		sectionsPanel.Position = UDim2.new(0,panelX, 0,panelY)
		sectionsPanel.Size = UDim2.new(0,panelW, 0,panelH)
	end

	task.defer(updateLayout)
	mainFrame:GetPropertyChangedSignal("AbsolutePosition"):Connect(updateLayout)
	mainFrame:GetPropertyChangedSignal("AbsoluteSize"):Connect(updateLayout)
	innerFrame:GetPropertyChangedSignal("AbsolutePosition"):Connect(updateLayout)
	innerFrame:GetPropertyChangedSignal("AbsoluteSize"):Connect(updateLayout)

	--==================================================
	-- Section selection visuals
	--==================================================
	local function applySectionSelected(btn, selected)
	btn.BackgroundColor3 = currentTheme.Inner
	btn.BackgroundTransparency = selected and INNER_T or 1
	end

	local function clearContent()
		closeDropdownMenu()
		ThemeTargets.Rows = {}
		for _, ch in ipairs(contentScroll:GetChildren()) do
			if ch:IsA("GuiObject") and ch ~= bottomSpacer then
				ch:Destroy()
			end
		end
		bottomSpacer.Parent = contentScroll
	end

	--==================================================
	-- Row factory (no pushing other rows when pressed)
	--==================================================
	local function createRow(height)
		local row = Instance.new("TextButton")
		row.AutoButtonColor = false
		row.Text = ""
		row.BackgroundTransparency = 1
		row.BorderSizePixel = 0
		row.Size = UDim2.new(1,0,0,height or ROW_H)
		row.ZIndex = 1011
		row.Parent = contentScroll

		local content = Instance.new("Frame")
		content.BorderSizePixel = 0
		content.BackgroundColor3 = currentTheme.Row
		content.BackgroundTransparency = ROW_BG_T
		content.Size = UDim2.new(1,0,1,0)
		content.Position = UDim2.new(0,0,0,0)
		content.ZIndex = 1011
		content.Parent = row

		local cc = Instance.new("UICorner")
		cc.CornerRadius = UDim.new(0, ROW_CORNER)
		cc.Parent = content

		local sc = Instance.new("UIScale")
		sc.Scale = 1
		sc.Parent = content

		return row, content, sc
	end

	local function pressButtonFX(content, scaleObj, baseColor)
		local bright = brighten(baseColor, 70)
		content.BackgroundColor3 = bright
		TweenService:Create(scaleObj, TweenInfo.new(0.06, Enum.EasingStyle.Quad, Enum.EasingDirection.Out), {Scale=0.985}):Play()
		task.delay(0.1, function()
			if content and content.Parent then
				content.BackgroundColor3 = baseColor
			end
			if scaleObj and scaleObj.Parent then
				TweenService:Create(scaleObj, TweenInfo.new(0.10, Enum.EasingStyle.Quad, Enum.EasingDirection.Out), {Scale=1}):Play()
			end
		end)
	end

	--==================================================
	-- Slider lock
	--==================================================
	local ACTIVE_SLIDER_ROW = nil

	--==================================================
	-- Renderers
	--==================================================
	local function runNotifyFromOpt(optNotify, stateForToggle)
		if typeof(optNotify) ~= "table" then return end
		if stateForToggle ~= nil then
			if not ShouldNotify(optNotify.Mode or "Both", stateForToggle) then return end
		end
		Notifier:Notify({
			Title = optNotify.Title or "Notification",
			Description = optNotify.Description or "",
			Duration = optNotify.Duration or 2.0,
		})
	end

	local function renderToggle(section, opt)
		local row, content = createRow(ROW_H)
		row.Name = "ToggleRow"

		local label = Instance.new("TextLabel")
		label.BackgroundTransparency = 1
		label.BorderSizePixel = 0
		label.Text = opt.Name or "Toggle"
		label.Font = ParseFont(opt.Font or section.Font or "SourceSansBold")
		label.TextSize = clampTextSize(opt.TextSize, 15)
		label.TextColor3 = ParseColor(opt.Color or section.Color or currentTheme.Text)
		label.TextXAlignment = Enum.TextXAlignment.Left
		label.TextYAlignment = Enum.TextYAlignment.Center
		label.Position = UDim2.new(0,10,0,0)
		label.Size = UDim2.new(1,-120,1,0)
		label.ZIndex = 1012
		label.Parent = content

		-- pill
		local DOT = 13
		local PAD = 2
		local pillW = DOT*2 + PAD*2
		local pillH = DOT + PAD*2

		local pill = Instance.new("Frame")
		pill.BorderSizePixel = 0
		pill.AnchorPoint = Vector2.new(1,0.5)
		pill.Position = UDim2.new(1,-10,0.5,0)
		pill.Size = UDim2.new(0,pillW,0,pillH)
		pill.ZIndex = 1012
		pill.Parent = content

		local pc = Instance.new("UICorner")
		pc.CornerRadius = UDim.new(1,0)
		pc.Parent = pill

		local dot = Instance.new("Frame")
		dot.BackgroundColor3 = Color3.fromRGB(255,255,255)
		dot.BorderSizePixel = 0
		dot.Size = UDim2.new(0,DOT,0,DOT)
		dot.ZIndex = 1013
		dot.Parent = pill

		local dc = Instance.new("UICorner")
		dc.CornerRadius = UDim.new(1,0)
		dc.Parent = dot

		local stateBox = {Value = (opt.Default == true)}

		local function setState(on, instant)
			stateBox.Value = on
			local leftX = PAD
			local rightX = pillW - DOT - PAD
			local x = on and rightX or leftX
			local col = on and currentTheme.Accent or currentTheme.PillOff

			if instant then
				pill.BackgroundColor3 = col
				dot.Position = UDim2.new(0,x,0.5,-math.floor(DOT/2))
			else
				TweenService:Create(pill, TweenInfo.new(0.12, Enum.EasingStyle.Quad, Enum.EasingDirection.Out), {BackgroundColor3=col}):Play()
				TweenService:Create(dot, TweenInfo.new(0.12, Enum.EasingStyle.Quad, Enum.EasingDirection.Out), {Position=UDim2.new(0,x,0.5,-math.floor(DOT/2))}):Play()
			end
		end

		setState(stateBox.Value, true)

		row.Activated:Connect(function()
			setState(not stateBox.Value, false)
			if typeof(opt.Callback) == "function" then opt.Callback(stateBox.Value) end

			if typeof(opt.NotifyOn) == "table" and stateBox.Value == true then
				runNotifyFromOpt(opt.NotifyOn, true)
			elseif typeof(opt.NotifyOff) == "table" and stateBox.Value == false then
				runNotifyFromOpt(opt.NotifyOff, false)
			else
				runNotifyFromOpt(opt.Notify, stateBox.Value)
			end
		end)

		table.insert(ThemeTargets.Rows, {Type="Toggle", Content=content, Label=label, Pill=pill, StateBox=stateBox})
	end

	local function renderButton(section, opt)
		local row, content, scaleObj = createRow(ROW_H)
		row.Name = "ButtonRow"

		local label = Instance.new("TextLabel")
		label.BackgroundTransparency = 1
		label.BorderSizePixel = 0
		label.Text = opt.Name or "Button"
		label.Font = ParseFont(opt.Font or section.Font or "SourceSansBold")
		label.TextSize = clampTextSize(opt.TextSize, 15)
		label.TextColor3 = ParseColor(opt.Color or section.Color or currentTheme.Text)
		label.TextXAlignment = Enum.TextXAlignment.Left
		label.TextYAlignment = Enum.TextYAlignment.Center
		label.Position = UDim2.new(0,10,0,0)
		label.Size = UDim2.new(1,-60,1,0)
		label.ZIndex = 1012
		label.Parent = content

		local icon = Instance.new("ImageLabel")
		icon.BackgroundTransparency = 1
		icon.BorderSizePixel = 0
		icon.Image = ASSET_FINGERPRINT
		icon.ScaleType = Enum.ScaleType.Fit
		icon.AnchorPoint = Vector2.new(1,0.5)
		icon.Position = UDim2.new(1,-10,0.5,0)
		icon.Size = UDim2.new(0,27,0,27) -- x1.5 permanent
		icon.ZIndex = 1013
		icon.Parent = content

		row.Activated:Connect(function()
			pressButtonFX(content, scaleObj, currentTheme.Row)
			if typeof(opt.Callback) == "function" then opt.Callback() end
			runNotifyFromOpt(opt.Notify, nil)
		end)

		table.insert(ThemeTargets.Rows, {Type="Button", Content=content, Label=label})
	end

	local function renderSlider(section, opt)
		local minV = tonumber(opt.Min) or 0
		local maxV = tonumber(opt.Max) or 100
		if maxV < minV then minV, maxV = maxV, minV end
		local inc = tonumber(opt.Increment) or 1
		if inc <= 0 then inc = 1 end

		local function snap(v)
			v = math.clamp(v, minV, maxV)
			local steps = (v - minV) / inc
			local snapped = minV + (math.floor(steps + 0.5) * inc)
			return math.clamp(snapped, minV, maxV)
		end

		local value = snap(tonumber(opt.Default) or minV)

		local row, content = createRow(ROW_H)
		row.Name = "SliderRow"

		local label = Instance.new("TextLabel")
		label.BackgroundTransparency = 1
		label.BorderSizePixel = 0
		label.Text = opt.Name or "Slider"
		label.Font = ParseFont(opt.Font or section.Font or "SourceSansBold")
		label.TextSize = clampTextSize(opt.TextSize, 15)
		label.TextColor3 = ParseColor(opt.Color or section.Color or currentTheme.Text)
		label.TextXAlignment = Enum.TextXAlignment.Left
		label.TextYAlignment = Enum.TextYAlignment.Center
		label.Position = UDim2.new(0,10,0,0)
		label.Size = UDim2.new(0,62,1,0)
		label.ZIndex = 1012
		label.Parent = content

		local valueBoxW = math.floor(48/1.3 + 0.5)
		local valueBox = Instance.new("Frame")
		valueBox.BorderSizePixel = 0
		valueBox.BackgroundColor3 = currentTheme.ValueBox
		valueBox.AnchorPoint = Vector2.new(1,0.5)
		valueBox.Position = UDim2.new(1,-10,0.5,0)
		valueBox.Size = UDim2.new(0,valueBoxW,0,18)
		valueBox.ZIndex = 1012
		valueBox.Parent = content

		local vbc = Instance.new("UICorner")
		vbc.CornerRadius = UDim.new(0,6)
		vbc.Parent = valueBox

		local valueInput = Instance.new("TextBox")
		valueInput.BackgroundTransparency = 1
		valueInput.BorderSizePixel = 0
		valueInput.ClearTextOnFocus = false
		valueInput.Text = tostring(value)
		valueInput.Font = Enum.Font.SourceSansBold
		valueInput.TextSize = clampTextSize(opt.ValueTextSize, 14)
		valueInput.TextColor3 = Color3.fromRGB(255,255,255)
		valueInput.TextXAlignment = Enum.TextXAlignment.Center
		valueInput.TextYAlignment = Enum.TextYAlignment.Center
		valueInput.Size = UDim2.new(1,0,1,0)
		valueInput.ZIndex = 1013
		valueInput.Parent = valueBox

		local trackH = 3 -- “2 раза меньше” от ~6
		local knobD = math.floor(trackH * 1.5 + 0.5)

		local track = Instance.new("Frame")
		track.BorderSizePixel = 0
		track.BackgroundColor3 = currentTheme.PillOff
		track.AnchorPoint = Vector2.new(0,0.5)
		track.Position = UDim2.new(0, 10+62+8, 0.5, 0)
		track.Size = UDim2.new(0, 120, 0, trackH) -- will be recalculated
		track.ZIndex = 1012
		track.Parent = content

		local tc = Instance.new("UICorner")
		tc.CornerRadius = UDim.new(1,0)
		tc.Parent = track

		local fill = Instance.new("Frame")
		fill.BorderSizePixel = 0
		fill.BackgroundColor3 = currentTheme.Fill
		fill.Size = UDim2.new(0,0,1,0)
		fill.ZIndex = 1013
		fill.Parent = track

		local fc = Instance.new("UICorner")
		fc.CornerRadius = UDim.new(1,0)
		fc.Parent = fill

		local knob = Instance.new("ImageButton")
		knob.AutoButtonColor = false
		knob.BorderSizePixel = 0
		knob.BackgroundColor3 = Color3.fromRGB(255,255,255)
		knob.AnchorPoint = Vector2.new(0.5,0.5)
		knob.Position = UDim2.new(0,0,0.5,0)
		knob.Size = UDim2.new(0,knobD,0,knobD)
		knob.ZIndex = 1014
		knob.Parent = track

		local kc = Instance.new("UICorner")
		kc.CornerRadius = UDim.new(1,0)
		kc.Parent = knob

		local function layoutTrack()
			local cw = content.AbsoluteSize.X
			if cw <= 0 then return end
			local valueLeft = cw - 10 - valueBoxW
			local trackRight = valueLeft - 10 -- 10px from value box
			local trackLeft = 10 + 62 + 8
			local trackW = trackRight - trackLeft
			if trackW < 40 then trackW = 40 end
			track.Position = UDim2.new(0,trackLeft,0.5,0)
			track.Size = UDim2.new(0,trackW,0,trackH)
		end

		content:GetPropertyChangedSignal("AbsoluteSize"):Connect(layoutTrack)
		task.defer(layoutTrack)

		local function setFromAlpha(a, fire)
			a = math.clamp(a, 0, 1)
			local raw = minV + a * (maxV - minV)
			local newVal = snap(raw)

			if newVal ~= value then
				value = newVal
				if fire and typeof(opt.Callback) == "function" then
					opt.Callback(value)
				end
			end

			valueInput.Text = tostring(value)

			local w = track.AbsoluteSize.X
			if w <= 1 then return end
			local x = math.clamp(((value - minV) / (maxV - minV)) * w, 0, w)
			knob.Position = UDim2.new(0,x,0.5,0)
			fill.Size = UDim2.new(0,x,1,0)
		end

		local function setByScreenX(screenX, fire)
			local left = track.AbsolutePosition.X
			local w = track.AbsoluteSize.X
			if w <= 1 then return end
			setFromAlpha((screenX-left)/w, fire)
		end

		task.defer(function()
			local a = 0
			if maxV ~= minV then a = (value - minV) / (maxV - minV) end
			setFromAlpha(a, false)
		end)

		local dragging = false
		local activeInput = nil

		local function beginDrag(input)
			if ACTIVE_SLIDER_ROW ~= nil and ACTIVE_SLIDER_ROW ~= row then return end
			ACTIVE_SLIDER_ROW = row
			dragging = true
			activeInput = input
			setByScreenX(input.Position.X, true)

			input.Changed:Connect(function()
				if input.UserInputState == Enum.UserInputState.End then
					if activeInput == input then
						dragging = false
						activeInput = nil
						if ACTIVE_SLIDER_ROW == row then ACTIVE_SLIDER_ROW = nil end
					end
				end
			end)
		end

		knob.InputBegan:Connect(function(input)
			if input.UserInputType == Enum.UserInputType.MouseButton1 or input.UserInputType == Enum.UserInputType.Touch then
				beginDrag(input)
			end
		end)
		track.InputBegan:Connect(function(input)
			if input.UserInputType == Enum.UserInputType.MouseButton1 or input.UserInputType == Enum.UserInputType.Touch then
				beginDrag(input)
			end
		end)

		UserInputService.InputChanged:Connect(function(input)
			if dragging and activeInput and input == activeInput and ACTIVE_SLIDER_ROW == row then
				setByScreenX(input.Position.X, true)
			end
		end)

		valueInput.FocusLost:Connect(function()
			local n = tonumber(valueInput.Text)
			if n == nil then
				valueInput.Text = tostring(value)
				return
			end
			n = snap(n)
			if n ~= value then
				value = n
				if typeof(opt.Callback) == "function" then opt.Callback(value) end
			end
			local a = 0
			if maxV ~= minV then a = (value - minV) / (maxV - minV) end
			setFromAlpha(a, false)
		end)

		table.insert(ThemeTargets.Rows, {Type="Slider", Content=content, Label=label, Track=track, Fill=fill, ValueBox=valueBox})
	end

	local function renderDropdown(section, opt)
		local options = opt.Options
		if typeof(options) ~= "table" then options = {"1","2","3"} end

		local multi = (opt.Multi == true)

		local selectedSingle = opt.Default
		if selectedSingle == nil then selectedSingle = options[1] end

		local selectedMulti = {}
		if multi then
			if typeof(opt.Default) == "table" then
				for _,v in ipairs(opt.Default) do selectedMulti[tostring(v)] = true end
			else
				selectedMulti[tostring(options[1])] = true
			end
		end

		local function displayText()
			if not multi then return tostring(selectedSingle) end
			local arr = {}
			for _,v in ipairs(options) do
				if selectedMulti[tostring(v)] then table.insert(arr, tostring(v)) end
			end
			if #arr == 0 then return "None" end
			if #arr > 3 then return tostring(#arr).." selected" end
			return table.concat(arr, ", ")
		end

		local row, content = createRow(ROW_H)
		row.Name = "DropdownRow"

		local label = Instance.new("TextLabel")
		label.BackgroundTransparency = 1
		label.BorderSizePixel = 0
		label.Text = opt.Name or "Dropdown"
		label.Font = ParseFont(opt.Font or section.Font or "SourceSansBold")
		label.TextSize = clampTextSize(opt.TextSize, 15)
		label.TextColor3 = ParseColor(opt.Color or section.Color or currentTheme.Text)
		label.TextXAlignment = Enum.TextXAlignment.Left
		label.TextYAlignment = Enum.TextYAlignment.Center
		label.Position = UDim2.new(0,10,0,0)
		label.Size = UDim2.new(1,-130,1,0)
		label.ZIndex = 1012
		label.Parent = content

		local selectBtn = Instance.new("TextButton")
		selectBtn.AutoButtonColor = false
		selectBtn.BorderSizePixel = 0
		selectBtn.BackgroundColor3 = currentTheme.ValueBox
		selectBtn.BackgroundTransparency = 0
		selectBtn.Text = displayText()
		selectBtn.Font = Enum.Font.SourceSansBold
		selectBtn.TextSize = clampTextSize(opt.ValueTextSize, 14)
		selectBtn.TextColor3 = Color3.fromRGB(255,255,255)
		selectBtn.AnchorPoint = Vector2.new(1,0.5)
		selectBtn.Position = UDim2.new(1,-10,0.5,0)
		selectBtn.Size = UDim2.new(0,110,0,22)
		selectBtn.ZIndex = 1013
		selectBtn.Parent = content

		local sc = Instance.new("UICorner")
		sc.CornerRadius = UDim.new(0,6)
		sc.Parent = selectBtn

		local function fireCallback()
			if typeof(opt.Callback) ~= "function" then return end
			if not multi then
				opt.Callback(selectedSingle)
			else
				local out = {}
				for _,v in ipairs(options) do
					if selectedMulti[tostring(v)] then table.insert(out, tostring(v)) end
				end
				opt.Callback(out)
			end
		end

		local function openMenu()
			closeDropdownMenu()

			local menu = Instance.new("Frame")
			menu.BorderSizePixel = 0
			menu.BackgroundColor3 = currentTheme.ValueBox
			menu.BackgroundTransparency = 0.3 -- semi transparent
			menu.ZIndex = 5000
			menu.Parent = screenGui
			openDropdownMenu = menu

			local mc = Instance.new("UICorner")
			mc.CornerRadius = UDim.new(0,8)
			mc.Parent = menu

			local pad = Instance.new("UIPadding")
			pad.PaddingTop = UDim.new(0,6)
			pad.PaddingBottom = UDim.new(0,6)
			pad.PaddingLeft = UDim.new(0,6)
			pad.PaddingRight = UDim.new(0,6)
			pad.Parent = menu

			local maxVisible = 6
			local itemH = 22
			local itemGap = 4
			local visible = math.min(#options, maxVisible)
			local viewH = (visible * itemH) + ((visible-1)*itemGap)

			local host
			if #options > maxVisible then
				local s = Instance.new("ScrollingFrame")
				s.BackgroundTransparency = 1
				s.BorderSizePixel = 0
				s.ScrollBarThickness = 0
				s.ScrollingDirection = Enum.ScrollingDirection.Y
				s.ElasticBehavior = Enum.ElasticBehavior.WhenScrollable
				s.AutomaticCanvasSize = Enum.AutomaticSize.Y
				s.CanvasSize = UDim2.new(0,0,0,0)
				s.Size = UDim2.new(1,0,0,viewH)
				s.ZIndex = 5001
				s.Parent = menu
				host = s
			else
				local f = Instance.new("Frame")
				f.BackgroundTransparency = 1
				f.BorderSizePixel = 0
				f.Size = UDim2.new(1,0,0,viewH)
				f.ZIndex = 5001
				f.Parent = menu
				host = f
			end

			local list = Instance.new("UIListLayout")
			list.SortOrder = Enum.SortOrder.LayoutOrder
			list.Padding = UDim.new(0,itemGap)
			list.Parent = host

			for i,optVal in ipairs(options) do
				local s = tostring(optVal)

				local b = Instance.new("TextButton")
				b.AutoButtonColor = false
				b.BorderSizePixel = 0
				b.BackgroundColor3 = currentTheme.Row
				b.BackgroundTransparency = ROW_BG_T
				b.Text = s
				b.Font = Enum.Font.SourceSansBold
				b.TextSize = clampTextSize(opt.ValueTextSize, 14)
				b.TextColor3 = Color3.fromRGB(255,255,255)
				b.Size = UDim2.new(1,0,0,itemH)
				b.LayoutOrder = i
				b.ZIndex = 5002
				b.Parent = host

				local bc = Instance.new("UICorner")
				bc.CornerRadius = UDim.new(0,6)
				bc.Parent = b

				local function refresh()
					if not multi then
						b.BackgroundColor3 = (s == tostring(selectedSingle)) and brighten(currentTheme.Row, 25) or currentTheme.Row
					else
						b.BackgroundColor3 = (selectedMulti[s] == true) and brighten(currentTheme.Row, 25) or currentTheme.Row
					end
				end
				refresh()

				b.Activated:Connect(function()
					if not multi then
						selectedSingle = s
						selectBtn.Text = displayText()
						fireCallback()
						closeDropdownMenu()
						return
					end
					selectedMulti[s] = not selectedMulti[s]
					selectBtn.Text = displayText()
					fireCallback()
					refresh()
				end)
			end

			local pos = selectBtn.AbsolutePosition
			local size = selectBtn.AbsoluteSize
			menu.Size = UDim2.new(0,180,0,6+viewH+6)
			menu.Position = UDim2.new(0, pos.X + size.X + 5, 0, pos.Y)
		end

		selectBtn.Activated:Connect(function()
			if openDropdownMenu then closeDropdownMenu() else openMenu() end
		end)

		table.insert(ThemeTargets.Rows, {Type="Dropdown", Content=content, Label=label, SelectBtn=selectBtn})
	end

	local function renderTextBox(section, opt)
		local row, content = createRow(ROW_H)
		row.Name = "TextBoxRow"

		local label = Instance.new("TextLabel")
		label.BackgroundTransparency = 1
		label.BorderSizePixel = 0
		label.Text = opt.Name or "Text Box"
		label.Font = ParseFont(opt.Font or section.Font or "SourceSansBold")
		label.TextSize = clampTextSize(opt.TextSize, 15)
		label.TextColor3 = ParseColor(opt.Color or section.Color or currentTheme.Text)
		label.TextXAlignment = Enum.TextXAlignment.Left
		label.TextYAlignment = Enum.TextYAlignment.Center
		label.Position = UDim2.new(0,10,0,0)
		label.Size = UDim2.new(1,-140,1,0)
		label.ZIndex = 1012
		label.Parent = content

		local boxW = 111
		local box = Instance.new("Frame")
		box.BorderSizePixel = 0
		box.BackgroundColor3 = currentTheme.ValueBox
		box.AnchorPoint = Vector2.new(1,0.5)
		box.Position = UDim2.new(1,-10,0.5,0)
		box.Size = UDim2.new(0,boxW,0,22)
		box.ZIndex = 1012
		box.Parent = content

		local bc = Instance.new("UICorner")
		bc.CornerRadius = UDim.new(0,6)
		bc.Parent = box

		local input = Instance.new("TextBox")
		input.BackgroundTransparency = 1
		input.BorderSizePixel = 0
		input.ClearTextOnFocus = false
		input.Font = Enum.Font.SourceSansBold
		input.TextSize = clampTextSize(opt.ValueTextSize, 14)
		input.TextXAlignment = Enum.TextXAlignment.Center
		input.TextYAlignment = Enum.TextYAlignment.Center
		input.Size = UDim2.new(1,0,1,0)
		input.ZIndex = 1013
		input.Parent = box

		local def = opt.Default
		if def == nil then def = "" end
		if tostring(def) == "" then
			input.Text = ""
			input.PlaceholderText = "..."
		else
			input.Text = tostring(def)
			input.PlaceholderText = "..."
		end

		input.PlaceholderColor3 = Color3.fromRGB(255,255,255)
		input.TextColor3 = Color3.fromRGB(255,255,255)
		input.TextTransparency = 0.45

		input.Focused:Connect(function()
			input.TextTransparency = 0
		end)
		input.FocusLost:Connect(function()
			input.TextTransparency = 0.45
			if typeof(opt.Callback) == "function" then opt.Callback(input.Text) end
		end)

		table.insert(ThemeTargets.Rows, {Type="TextBox", Content=content, Label=label, InputBox=box})
	end

	local function renderLabel(section, opt)
		local lines = tonumber(opt.Lines) or 3
		if lines < 1 then lines = 1 end
		local rowH = 16 + (lines * 14)

		local row, content = createRow(rowH)
		row.Name = "LabelRow"

		local text = Instance.new("TextLabel")
		text.BackgroundTransparency = 1
		text.BorderSizePixel = 0
		text.TextWrapped = true
		text.Text = opt.Text or "Label"
		text.Font = ParseFont(opt.Font or section.Font or "SourceSans")
		text.TextSize = clampTextSize(opt.TextSize, 13)
		text.TextColor3 = ParseColor(opt.Color or section.Color or currentTheme.Muted)
		text.TextXAlignment = Enum.TextXAlignment.Left
		text.TextYAlignment = Enum.TextYAlignment.Top
		text.Position = UDim2.new(0,10,0,10)
		text.Size = UDim2.new(1,-20,1,-20)
		text.ZIndex = 1012
		text.Parent = content

		table.insert(ThemeTargets.Rows, {Type="Label", Content=content, LabelMuted=text})
	end

	local function renderHeadText(section, opt)
		local row = Instance.new("Frame")
		row.BackgroundTransparency = 1
		row.BorderSizePixel = 0
		row.Size = UDim2.new(1,0,0,30)
		row.ZIndex = 1011
		row.Parent = contentScroll

		local head = Instance.new("TextLabel")
		head.BackgroundTransparency = 1
		head.BorderSizePixel = 0
		head.Text = opt.Text or "Head Text"
		head.Font = ParseFont(opt.Font or section.Font or "SourceSansBold")
		head.TextSize = clampTextSize(opt.TextSize, 16)
		head.TextColor3 = ParseColor(opt.Color or section.Color or currentTheme.Text)
		head.TextXAlignment = Enum.TextXAlignment.Center
		head.TextYAlignment = Enum.TextYAlignment.Center
		head.Size = UDim2.new(0,110,0,12)
		head.AnchorPoint = Vector2.new(0.5,0.5)
		head.Position = UDim2.new(0.5,0,0.5,0)
		head.ZIndex = 1013
		head.Parent = row

		local showLines = (opt.Lines ~= false)
		local lineColor = ParseColor(opt.LineColor or currentTheme.Lines)

		local leftLine = Instance.new("Frame")
		leftLine.BorderSizePixel = 0
		leftLine.BackgroundColor3 = lineColor
		leftLine.Visible = showLines
		leftLine.ZIndex = 1012
		leftLine.Parent = row

		local rightLine = Instance.new("Frame")
		rightLine.BorderSizePixel = 0
		rightLine.BackgroundColor3 = lineColor
		rightLine.Visible = showLines
		rightLine.ZIndex = 1012
		rightLine.Parent = row

		local function layoutLines()
			if not showLines then return end
			local w = row.AbsoluteSize.X
			if w <= 0 then return end
			local innerPad = 10
			local gap = 10 -- as requested
			local headW = 110
			local available = w - (innerPad*2) - headW - (gap*2)
			local lineW = math.max(10, math.floor(available/2))
			leftLine.Size = UDim2.new(0,lineW,0,2)
			leftLine.Position = UDim2.new(0, innerPad, 0.5, -1)
			rightLine.Size = UDim2.new(0,lineW,0,2)
			rightLine.Position = UDim2.new(1, -innerPad-lineW, 0.5, -1)
		end

		task.defer(layoutLines)
		row:GetPropertyChangedSignal("AbsoluteSize"):Connect(layoutLines)

		table.insert(ThemeTargets.Rows, {Type="HeadText", Content=nil, Label=head, LeftLine=leftLine, RightLine=rightLine})
	end

	--==================================================
	-- Section render
	--==================================================
	local function renderSection(section)
		clearContent()
		for _, el in ipairs(section._elements) do
			if el.Type == "Toggle" then renderToggle(section, el.Opt)
			elseif el.Type == "Button" then renderButton(section, el.Opt)
			elseif el.Type == "Slider" then renderSlider(section, el.Opt)
			elseif el.Type == "Dropdown" then renderDropdown(section, el.Opt)
			elseif el.Type == "TextBox" then renderTextBox(section, el.Opt)
			elseif el.Type == "Label" then renderLabel(section, el.Opt)
			elseif el.Type == "HeadText" then renderHeadText(section, el.Opt)
			end
		end
		ApplyTheme(currentTheme)
	end

	local function setSelected(idx)
		if Window._selectedIndex == idx then return end
		if Window._selectedIndex then
			local old = Window._sectionButtons[Window._selectedIndex]
			if old then applySectionSelected(old, false) end
		end
		Window._selectedIndex = idx
		local btn = Window._sectionButtons[idx]
		if btn then applySectionSelected(btn, true) end
		local section = Window.Sections[idx]
		if section then renderSection(section) end
	end

	--==================================================
	-- AddSection API
	--==================================================
	function Window:AddSection(opt)
		opt = opt or {}
		local section = {}
		section.Name = opt.Name or "Section"
		section.Font = ParseFont(opt.Font or "SourceSans")
		section.Color = ParseColor(opt.Color or "#ffffff")
		section._elements = {}

		function section:AddToggle(t) table.insert(self._elements, {Type="Toggle", Opt=t or {}}) end
		function section:AddButton(t) table.insert(self._elements, {Type="Button", Opt=t or {}}) end
		function section:AddSlider(t) table.insert(self._elements, {Type="Slider", Opt=t or {}}) end
		function section:AddDropdown(t) table.insert(self._elements, {Type="Dropdown", Opt=t or {}}) end
		function section:AddTextBox(t) table.insert(self._elements, {Type="TextBox", Opt=t or {}}) end
		function section:AddLabel(t) table.insert(self._elements, {Type="Label", Opt=t or {}}) end
		function section:AddHeadText(t) table.insert(self._elements, {Type="HeadText", Opt=t or {}}) end

		table.insert(Window.Sections, section)
		local idx = #Window.Sections

		local b = Instance.new("TextButton")
		b.AutoButtonColor = false
		b.BorderSizePixel = 0
		b.Text = section.Name
		b.Font = Enum.Font.SourceSans
		b.TextSize = 12 -- smaller, non-bold
		b.TextColor3 = currentTheme.Title
		b.TextXAlignment = Enum.TextXAlignment.Left
		b.Size = UDim2.new(1,0,0,24)
		b.LayoutOrder = idx
		b.ZIndex = 1003
		b.Parent = sectionsPanel

		local pad = Instance.new("UIPadding")
		pad.PaddingLeft = UDim.new(0,12) -- +1 left/right from prior
		pad.PaddingRight = UDim.new(0,2)
		pad.Parent = b

		local c = Instance.new("UICorner")
		c.CornerRadius = UDim.new(0, INNER_CORNER)
		c.Parent = b

		applySectionSelected(b, false)

		b.Activated:Connect(function()
			setSelected(idx)
		end)

		Window._sectionButtons[idx] = b
		table.insert(ThemeTargets.SectionButtons, b)

		if idx == 1 then setSelected(1) end
		return section
	end

	--==================================================
	-- Optional Theme section (15 themes)
	--==================================================
	if settings.ThemeSection == true then
		local themeSection = Window:AddSection({Name="Themes", Font="SourceSans", Color="#ffffff"})
		local names = {}
		for _,t in ipairs(Themes) do table.insert(names, t.Name) end

		themeSection:AddDropdown({
			Name="Theme",
			Options=names,
			Default=Themes[1].Name,
			Multi=false,
			Callback=function(name)
				Window:SetTheme(name)
			end
		})
	end

	-- apply initial theme
	ApplyTheme(currentTheme)

	return Window
end

return setmetatable({}, XuilanLib)
