function table_to_string(tbl)
    local result = "{"
    for k, v in pairs(tbl) do
        -- Check the key type (ignore any numerical keys - assume its an array)
        if type(k) == "string" then
            result = result.."[\""..k.."\"]".."="
        end

        -- Check the value type
        if type(v) == "table" then
            result = result..table_to_string(v)
        elseif type(v) == "boolean" then
            result = result..tostring(v)
        elseif type(v) == "number" then
            result = result..tostring(v)
        elseif v == nil then
            result = result.."NIL"
        else
            result = result.."\""..v.."\""
        end
        result = result..","
    end
    -- Remove leading commas from the result
    if result ~= "" then
        result = result:sub(1, result:len()-1)
    end
    return result.."}"
end

-- print(myself, table_to_string(colors))

local floor = math.floor
function bxor(a, b)
  local r = 0
  for i = 0, 31 do
    local x = a / 2 + b / 2
    if x ~= floor(x) then
      r = r + 2^i
    end
    a = floor(a / 2)
    b = floor(b / 2)
  end
  return r
end

x = 0
for i = 1, 13 do
    if i ~= myself then
        x = x ~ (colors[i] - 1)
    end
end

-- print('x', x)

if myself == 13 then
    x2 = 0
    for i = 0, 11 do
        if (x & (1 << i)) ~= 0 then
            x2 = 1 - x2
        end
    end
    my_bool = (x2 ~= 0)
else
    bit = x & (1 << (myself - 1))
    my_bool = (bit ~= 0)
end

-- print('my_bool', my_bool)

hands = raise(my_bool)

-- print(myself, table_to_string(colors), table_to_string(hands))

answer = 0
for i = 1, 12 do
    if myself == i then
        
    else
        h = 0
        if hands[i] then
            h = 1 << (i - 1)
        end
        a = (x ~ h ~ (colors[i] - 1)) & (1 << (i - 1))
        answer = answer | a
    end
end

-- print("answer1", answer)

if myself ~= 13 then
    a = x ~ (colors[13] - 1) ~ answer
    count = 0
    for i = 1, 12 do
        if (a & (1 << (i - 1))) ~= 0 then
            count = 1 - count
        end
    end
    h = 0
    if hands[13] then
        h = 1
    end
    if count ~= h then
        answer = answer | (1 << (myself - 1))
    end
end

answer = answer + 1

-- print("answer2", answer)
