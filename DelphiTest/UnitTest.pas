unit UnitTest;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls, Vcl.ToolWin, Vcl.ComCtrls;

type
  TFormTest = class(TForm)
    StatusBar1: TStatusBar;
    ToolBar1: TToolBar;
    Edit1: TEdit;
    Edit2: TEdit;
    Button1: TButton;
    procedure Button1Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  FormTest: TFormTest;

implementation

{$R *.dfm}

procedure TFormTest.Button1Click(Sender: TObject);
begin
  Edit2.Text := Edit1.Text;
end;

end.
