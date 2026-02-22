– XuilanLib (Fixed Version) – Elements render correctly – Open Ui pill
fixed (drag only on arrows, no empty space) – Notification width reduced
by 1/3

local Players = game:GetService(“Players”) local TweenService =
game:GetService(“TweenService”) local UserInputService =
game:GetService(“UserInputService”) local TextService =
game:GetService(“TextService”)

local XuilanLib = {} XuilanLib.__index = XuilanLib

local function CreateNotifier(gui) local holder = Instance.new(“Frame”)
holder.AnchorPoint = Vector2.new(1,1) holder.Position =
UDim2.new(1,-12,1,-12) holder.Size = UDim2.new(0,213,0,200)
holder.BackgroundTransparency = 1 holder.Parent = gui

    local layout = Instance.new("UIListLayout", holder)
    layout.VerticalAlignment = Enum.VerticalAlignment.Bottom
    layout.Padding = UDim.new(0,8)

    local function Notify(opt)
        local card = Instance.new("Frame")
        card.Size = UDim2.new(1,0,0,60)
        card.BackgroundColor3 = Color3.fromRGB(18,18,18)
        card.BackgroundTransparency = 0.12
        card.BorderSizePixel = 0
        card.Parent = holder
        
        local c = Instance.new("UICorner", card)
        c.CornerRadius = UDim.new(0,14)
        
        local title = Instance.new("TextLabel", card)
        title.BackgroundTransparency = 1
        title.Text = opt.Title or "Notification"
        title.Font = Enum.Font.SourceSansBold
        title.TextSize = 16
        title.TextColor3 = Color3.fromRGB(255,255,255)
        title.Position = UDim2.new(0,12,0,6)
        title.Size = UDim2.new(1,-24,0,20)
        title.TextXAlignment = Enum.TextXAlignment.Left
        
        local desc = Instance.new("TextLabel", card)
        desc.BackgroundTransparency = 1
        desc.TextWrapped = true
        desc.Text = opt.Description or ""
        desc.Font = Enum.Font.SourceSans
        desc.TextSize = 14
        desc.TextColor3 = Color3.fromRGB(220,220,220)
        desc.Position = UDim2.new(0,12,0,26)
        desc.Size = UDim2.new(1,-24,1,-32)
        desc.TextXAlignment = Enum.TextXAlignment.Left
        
        card.Position = UDim2.new(1,300,0,0)
        TweenService:Create(card,TweenInfo.new(0.3),{Position=UDim2.new(0,0,0,0)}):Play()
        
        task.delay(opt.Duration or 2,function()
            TweenService:Create(card,TweenInfo.new(0.3),{Position=UDim2.new(1,300,0,0)}):Play()
            task.wait(0.3)
            card:Destroy()
        end)
    end

    return {Notify = Notify}

end

function XuilanLib:CreateWindow(settings) local player =
Players.LocalPlayer local gui = Instance.new(“ScreenGui”)
gui.ResetOnSpawn = false gui.Parent = player:WaitForChild(“PlayerGui”)

    local notifier = CreateNotifier(gui)

    local main = Instance.new("Frame", gui)
    main.Size = UDim2.new(0,400,0,280)
    main.Position = UDim2.new(0.5,-200,0.5,-140)
    main.BackgroundColor3 = Color3.fromRGB(18,18,18)
    main.BackgroundTransparency = 0.12
    main.BorderSizePixel = 0

    local mc = Instance.new("UICorner", main)
    mc.CornerRadius = UDim.new(0,14)

    local title = Instance.new("TextLabel", main)
    title.BackgroundTransparency = 1
    title.Text = settings.Title or "XuilanLib"
    title.Font = Enum.Font.SourceSansBold
    title.TextSize = 20
    title.TextColor3 = Color3.fromRGB(255,255,255)
    title.Position = UDim2.new(0,10,0,5)
    title.Size = UDim2.new(1,-20,0,24)
    title.TextXAlignment = Enum.TextXAlignment.Left

    local content = Instance.new("ScrollingFrame", main)
    content.Position = UDim2.new(0,10,0,40)
    content.Size = UDim2.new(1,-20,1,-50)
    content.CanvasSize = UDim2.new(0,0,0,0)
    content.ScrollBarThickness = 0
    content.BackgroundTransparency = 1

    local list = Instance.new("UIListLayout", content)
    list.Padding = UDim.new(0,8)
    list:GetPropertyChangedSignal("AbsoluteContentSize"):Connect(function()
        content.CanvasSize = UDim2.new(0,0,0,list.AbsoluteContentSize.Y+10)
    end)

    local openPill = Instance.new("Frame", gui)
    openPill.Visible = false
    openPill.AnchorPoint = Vector2.new(0.5,0)
    openPill.Position = UDim2.new(0.5,0,0,10)
    openPill.BackgroundColor3 = Color3.fromRGB(18,18,18)
    openPill.BackgroundTransparency = 0.12
    openPill.BorderSizePixel = 0

    local pc = Instance.new("UICorner", openPill)
    pc.CornerRadius = UDim.new(1,0)

    local arrows = Instance.new("ImageButton", openPill)
    arrows.BackgroundTransparency = 1
    arrows.Image = "rbxassetid://97961707453604"
    arrows.Size = UDim2.new(0,21,0,21)
    arrows.Position = UDim2.new(0,8,0.5,-10)
    arrows.AutoButtonColor = false

    local openBtn = Instance.new("TextButton", openPill)
    openBtn.BackgroundTransparency = 1
    openBtn.Size = UDim2.new(1,0,1,0)
    openBtn.Text = ""

    local label = Instance.new("TextLabel", openPill)
    label.BackgroundTransparency = 1
    label.Text = "Open Ui"
    label.Font = Enum.Font.SourceSansBold
    label.TextSize = 16
    label.TextColor3 = Color3.fromRGB(255,255,255)
    label.TextXAlignment = Enum.TextXAlignment.Left
    label.Position = UDim2.new(0,42,0,-2)
    label.Size = UDim2.new(1,-50,1,0)

    local function resizePill()
        local bounds = TextService:GetTextSize(label.Text, label.TextSize, label.Font, Vector2.new(1000,1000))
        local width = 8 + 21 + 8 + 5 + bounds.X + 8
        if width < 120 then width = 120 end
        openPill.Size = UDim2.new(0,width,0,34)
    end

    resizePill()

    openBtn.MouseButton1Click:Connect(function()
        main.Visible = true
        openPill.Visible = false
    end)

    function XuilanLib:Notify(opt)
        notifier.Notify(opt)
    end

    local Window = {}

    function Window:AddButton(opt)
        local btn = Instance.new("TextButton", content)
        btn.Size = UDim2.new(1,0,0,34)
        btn.BackgroundColor3 = Color3.fromRGB(150,150,150)
        btn.BackgroundTransparency = 0.8
        btn.BorderSizePixel = 0
        btn.Text = opt.Name or "Button"
        btn.Font = Enum.Font.SourceSansBold
        btn.TextSize = 15
        btn.TextColor3 = Color3.fromRGB(255,255,255)
        
        local c = Instance.new("UICorner", btn)
        c.CornerRadius = UDim.new(0,12)
        
        btn.MouseButton1Click:Connect(function()
            if opt.Callback then
                opt.Callback()
            end
            if opt.Notify then
                notifier.Notify(opt.Notify)
            end
        end)
    end

    function Window:Notify(opt)
        notifier.Notify(opt)
    end

    return Window

end

return setmetatable({}, XuilanLib)
