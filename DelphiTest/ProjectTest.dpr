program ProjectTest;

uses
  Vcl.Forms,
  UnitTest in 'UnitTest.pas' {FormTest};

{$R *.res}

begin
  Application.Initialize;
  Application.MainFormOnTaskbar := True;
  Application.CreateForm(TFormTest, FormTest);
  Application.Run;
end.
