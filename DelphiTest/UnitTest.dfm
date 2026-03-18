object FormTest: TFormTest
  Left = 0
  Top = 0
  Caption = 'FormTest'
  ClientHeight = 441
  ClientWidth = 624
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Segoe UI'
  Font.Style = []
  TextHeight = 15
  object StatusBar1: TStatusBar
    Left = 0
    Top = 422
    Width = 624
    Height = 19
    Panels = <>
    ExplicitLeft = 320
    ExplicitTop = 232
    ExplicitWidth = 0
  end
  object ToolBar1: TToolBar
    Left = 0
    Top = 0
    Width = 624
    Height = 29
    Caption = 'ToolBar1'
    TabOrder = 1
    ExplicitLeft = 248
    ExplicitTop = 224
    ExplicitWidth = 150
  end
  object Edit1: TEdit
    Left = 320
    Top = 144
    Width = 121
    Height = 23
    TabOrder = 2
    Text = 'Edit1'
  end
  object Edit2: TEdit
    Left = 328
    Top = 192
    Width = 121
    Height = 23
    TabOrder = 3
    Text = 'Edit2'
  end
  object Button1: TButton
    Left = 328
    Top = 248
    Width = 75
    Height = 25
    Caption = 'Button1'
    TabOrder = 4
    OnClick = Button1Click
  end
end
