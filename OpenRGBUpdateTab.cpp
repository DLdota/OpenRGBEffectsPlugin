#include "OpenRGBUpdateTab.h"
#include "ORGBUpdatePlugin.h"

/*-----------------------------------------------------*\
| Updating is not a patching process                    |
| It just completely redownloads                        |
|                                                       |
| In the future having a patching process may be helpful|
\*-----------------------------------------------------*/

OpenRGBUpdateInfoPage::OpenRGBUpdateInfoPage(std::vector<std::string> UpdateVars, QWidget *Parent) :
    QFrame(Parent),
    ui(new Ui::OpenRGBUpdateInfoPageUi)
    {
        ui->setupUi(this);

        ui->CheckOrUpdateProgress->hide();
        ui->StatusMessage->hide();

        ui->GitCommitInfoTable->setColumnCount(5);
        QStringList HeaderNameList =
        {
            "Commit Date",
            "Commit Name",
            "Commit Comments",
            "Commit ID",
            "Build Status",
        };
        QStringList HeaderWidth =
        {
            "90",
            "350",
            "120",
            "70",
            "70",
        };

        ui->GitCommitInfoTable->setHorizontalHeaderLabels(HeaderNameList);
        ui->GitCommitInfoTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->GitCommitInfoTable->setSelectionMode(QAbstractItemView::NoSelection);
        ui->GitCommitInfoTable->setCornerButtonEnabled(false);
        ui->GitCommitInfoTable->setRowCount(12);

        for (int i = 0; i < 5; i++)
        {
            ui->GitCommitInfoTable->setColumnWidth(i, HeaderWidth[i].toInt());
        }
        OpenRGBUpdateInfoPage::CheckBranch = QString().fromStdString(UpdateVars[0]);
        OpenRGBUpdateInfoPage::Fork = QString().fromStdString(UpdateVars[1]);
    }

OpenRGBUpdateInfoPage::~OpenRGBUpdateInfoPage()
{
    delete ui;
}

void CreateMsgDialog(QString MSG)
{
    QDialog *MsgDialog = new QDialog();
    MsgDialog->setModal(true);
    MsgDialog->setMinimumSize(QSize(300,100));

    QLayout *MsgLayout = new QVBoxLayout(MsgDialog);
    MsgLayout->setAlignment(Qt::AlignCenter);

    if (ORGBPlugin::DarkTheme)
    {
        QPalette pal;
        pal.setColor(QPalette::WindowText, Qt::white);
        MsgDialog->setPalette(pal);
        QFile darkTheme(":/windows_dark.qss");
        darkTheme.open(QFile::ReadOnly);
        MsgDialog->setStyleSheet(darkTheme.readAll());
    }

    QLabel *MsgLabel = new QLabel();
    MsgLabel->setText(MSG);
    MsgLabel->setAlignment(Qt::AlignVCenter);
    MsgLabel->setAlignment(Qt::AlignHCenter);

    MsgLayout->addWidget(MsgLabel);
    MsgDialog->show();
}

QString ContentCheck(QString Result, QStringList SplitAt, std::vector<int> SplitWhere)
{
    for (int i = 0; i < SplitAt.size(); i++)
    {
        if (Result.contains(SplitAt[i]))
        {
            Result = Result.split(SplitAt[i])[SplitWhere[i]];
        }
        else
        {
            return "PARSING FAIL";
        }
    }
    return Result;
}

std::vector<QStringList> OpenRGBUpdateInfoPage::ParseCommitList(QString ContentToParse)
{
    ContentToParse = ContentToParse.replace("\n","");

    /*-------------------------------------------------------------------------------------------------------------------------------------------------------*\
    |  If you want to know the structure of Gitlabs website then see the table below (in the on_CheckButton_clicked funtion) or press F12 in most browsers    |
    \*-------------------------------------------------------------------------------------------------------------------------------------------------------*/
    QString ListOpening = ContentCheck(ContentToParse, {"list-unstyled content_list"},{1});
    if (ListOpening == "PARSING FAIL")
    {
        CreateMsgDialog("Error parsing initial commit list\nPlease contact @CoffeeIsLife on the discord");
        return {{"Parsing","Error"}};
    }
    std::vector<QStringList> CommitInfoList;

    QStringList CommitData = ListOpening.split("</div></li></ul>");
    for (int i = 0; i < CommitData.size() - 1; i++)
    {
        /*------------------------------------------------------------------------------------------------------------------*\
        | The following lines in this for loop will split the HTML source code into bit that contain the information wanted  |
        | It does an Opening split and a closing split using indexes to grab the proper section                              |
        \*------------------------------------------------------------------------------------------------------------------*/
        QString CommitDate           =     ContentCheck(CommitData[i],{" data-day=\"", "\"><span class="},{1,0});
        QString CommitHash           =     ContentCheck(CommitData[i],{("js-onboarding-commit-item \" href=\"/" + Fork + "/OpenRGB/-/commit/"), "\">"}, {1,0});
        QString CommitName           =     ContentCheck(CommitData[i],{("js-onboarding-commit-item \" href=\"/" + Fork + "/OpenRGB/-/commit/" + CommitHash + "\">"), "</a><span class=\""},{1,0});
        QString CommitComments       =     "No Comments";
        QString CommitID             =     ContentCheck(CommitData[i], {"<div class=\"label label-monospace monospace\">", "</div><button class=\""}, {1,0});
        QString CommitHasBuiltstr    =     ContentCheck(CommitData[i], {("class=\"ci-status-link ci-status-icon-"), " d-inline-flex \" title="}, {1,0});
        if (CommitData[i].contains("<pre class=\"commit-row-description gl-mb-3 js-toggle-content\">"))
        {
            /*------------------------------------------------------------------------------------------------------------------------------------------------*\
            | This can't go in the function because it has too many special cases that would be stupid to implement into the function as it is only used once  |
            \*------------------------------------------------------------------------------------------------------------------------------------------------*/
            CommitComments = CommitData[i].split("<pre class=\"commit-row-description gl-mb-3 js-toggle-content\">")[1].split("</pre></d")[0].replace("&#x000A;*","  ").replace("&#x000A;","");
            if (CommitComments.contains("Commits squashed"))
            {
                CommitComments = CommitComments.split("Commits squashed")[0];
            }
            if (CommitComments.contains("&lt;<a href=\"/cdn-cgi"))
            {
                CommitComments = CommitComments.split("&lt;<a href=\"/cdn-cgi")[0];
            }
            else if (CommitComments.contains("<a href=\""))
            {
                CommitComments = CommitComments.split("<a href=\"")[0];
            }
        }
        /*------------------------------------------------------------------------*\
        | Pack all the info into a sub list to be layed out on a data table later  |
        \*------------------------------------------------------------------------*/
        QStringList SubCommitInfo = {
            CommitDate,
            CommitName,
            CommitComments,
            CommitID,
            CommitHasBuiltstr,
            CommitHash
        };
        CommitInfoList.push_back(SubCommitInfo);
        SubCommitInfo.clear();
    }
    ContentToParse.clear();
    CommitData.clear();
    return CommitInfoList;
}

QByteArray OpenRGBUpdateInfoPage::GetPageSourceOrFile(QString link)
{
    QEventLoop GetPageSourceOrFile;
    QNetworkAccessManager Manager;
    QNetworkRequest RequestSourceOrFile((QUrl(link)));
    QNetworkReply *Reply = Manager.get(RequestSourceOrFile);
    connect(Reply, &QNetworkReply::finished, &GetPageSourceOrFile, &QEventLoop::quit);
    GetPageSourceOrFile.exec();
    QByteArray ReturnInput = Reply->readAll();
    GetPageSourceOrFile.deleteLater();

    return ReturnInput;
}

void OpenRGBUpdateInfoPage::UpdateProgressBar(bool Show, QString Text, int Value)
{
    /*------------------------------------------------------------------------------------------------*\
    | I could have done this differently but this is how I did it and I am too lazy to change it back  |
    | Text and Value default to NULL so that you can just disable the bar without other arguments      |
    \*------------------------------------------------------------------------------------------------*/
    bool AlreadyShowing = ui->CheckOrUpdateProgress->isHidden();

    if ((Show) && (AlreadyShowing))
    {
        ui->CheckOrUpdateProgress->show();
    }
    else if ((!Show) && (!AlreadyShowing))
    {
        ui->CheckOrUpdateProgress->hide();
    }
    if (Text != NULL)
    {
        ui->CheckOrUpdateProgress->setFormat(Text);
    }
    if (Value != NULL)
    {
        ui->CheckOrUpdateProgress->setValue(Value);
    }
}

void OpenRGBUpdateInfoPage::on_UpdateButton_clicked()
{
    /*-------------------------*\
    | idk how to use locks lol  |
    \*-------------------------*/
    OpenRGBUpdateInfoPage::OwnedByUpdateButton = true;

    /*---------------------------------------------------------------------------------------------------------------------*\
    | PipeLine links are stored as https://gitlab.com/CalcProgrammer1/OpenRGB/-/commit/Commit Hash/pipelines?ref=master     |
    | Because I am also storing the hash I can get the link pretty easily                                                   |
    |                                                                                                                       |
    | Downloading will go:                                                                                                  |
    |       Commit Hash page                                                                                                |
    |           PipeLine ID page                                                                                            |
    |           ^^^ For pipeline ID (hehe)                                                                                  |
    |               PipeLines Page (https://gitlab.com/CalcProgrammer1/OpenRGB/-/pipelines/ID)                              |
    |               ^^^ For Job ID                                                                                          |
    |                   Build page (https://gitlab.com/CalcProgrammer1/OpenRGB/-/jobs/ID)                                   |
    |                   ^^^ For Raw File link                                                                               |
    |                       Browse file page (https://gitlab.com/CalcProgrammer1/OpenRGB/-/jobs/883930863/artifacts/browse) |
    |                       /_Build (https://gitlab.com/CalcProgrammer1/OpenRGB/-/jobs/ID/artifacts/browse/_build/)         |
    |                           Obtain raw file link and download                                                           |
    \*---------------------------------------------------------------------------------------------------------------------*/

    /*-----------------------------------------------------------*\
    | Hide the status message box because it is no longer needed  |
    \*-----------------------------------------------------------*/
    if (!ui->StatusMessage->isHidden())
    {
        ui->StatusMessage->hide();
    }

    UpdateProgressBar(true,"Checking if Updates are checked",10);

    /*------------------------------------------------------*\
    | Make sure that it doesn't try to parse an empty list   |
    \*------------------------------------------------------*/
    if (OpenRGBUpdateInfoPage::Parsed.empty())
    {
        OpenRGBUpdateInfoPage::on_CheckButton_clicked();
        if (ContentMissing)
        {
            OpenRGBUpdateInfoPage::OwnedByUpdateButton = false;
            return;
        }
    }

    UpdateProgressBar(true,"Finding latest successful build",20);
    for (int i = 0; i < int(OpenRGBUpdateInfoPage::Parsed.size()); i++)
    {
        /*----------------------------------------------------------------------*\
        | Make sure that the build for that particular pipeline was a success    |
        \*----------------------------------------------------------------------*/
        if (OpenRGBUpdateInfoPage::Parsed[i][4] == "success")
        {
            /*-----------------------------------------------------------------*\
            | Turn the hash into a commit link                                  |
            \*-----------------------------------------------------------------*/
            OpenRGBUpdateInfoPage::CommitLink = "https://gitlab.com/" + Fork + "/OpenRGB/-/commit/" + OpenRGBUpdateInfoPage::Parsed[i][5] + "/pipelines?ref=" + CheckBranch;
            break;
        }
    }
    /*---------------*\
    | Get PipeLine ID |
    \*---------------*/

    UpdateProgressBar(true, "Finding Pipeline ID",30);
    QString OpenRGBUpdatePipeLinePagestr = OpenRGBUpdateInfoPage::GetPageSourceOrFile(OpenRGBUpdateInfoPage::CommitLink);
    QString PipeLineID = ContentCheck(OpenRGBUpdatePipeLinePagestr,{("<a href=\"/" + Fork + "/OpenRGB/-/pipelines/"),"\">#"}, {1,0});
    if (PipeLineID == "PARSING FAIL")
    {
        ContentMissing = true;
        UpdateProgressBar(false);
        CreateMsgDialog("Failed to fine pipeline ID\nPlease contact @CoffeeIsLife on discord");
        OpenRGBUpdateInfoPage::OwnedByUpdateButton = false;
        return;
    }
    /*------------------------------------------------------------*\
    | To start I will only offer Win64 and Lin64                   |
    | The following QStrings contain the link and the build status |
    \*------------------------------------------------------------*/
    QString PipeLinePageLink = "https://gitlab.com/" + Fork + "/OpenRGB/-/pipelines/" + PipeLineID;
    QString PipeLinePage = GetPageSourceOrFile(PipeLinePageLink);

    UpdateProgressBar(true, "Getting link for current OS",40);
    #ifdef _WIN32
    OpenRGBUpdateInfoPage::PreferedPlatform = "build_windows_64";
    #elif __linux__
    OpenRGBUpdateInfoPage::PreferedPlatform = "build_linux_64";
    #endif

    bool FoundPrefered = false;

    QStringList JobList;

    if (PipeLinePage.contains("data-html=\"true\" href=\"/" + Fork + "/OpenRGB/-/jobs/"))
    {
        JobList = PipeLinePage.split("data-html=\"true\" href=\"/" + Fork + "/OpenRGB/-/jobs/");
    }
    else if (!PipeLinePage.contains("data-html=\"true\" href=\"/" + Fork + "/OpenRGB/-/jobs/"))
    {
        ContentMissing = true;
        UpdateProgressBar(false);
        CreateMsgDialog("Failed to parse pipeline page\nPlease contact @CoffeeIsLife on discord");
        OpenRGBUpdateInfoPage::OwnedByUpdateButton = false;
        return;
    }

    for (int i = 0; i < JobList.count(); i++)
    {
        if (JobList[i].contains(PreferedPlatform))
        {
            qDebug() << "Found Prefered";
            FoundPrefered = true;
            /*---------------------------------------------------------------------------------------------------------------------------------------------------------------*\
            | There are some inconsistancies when it comes to checks due to needing to split at a specific instance of the substring rather than keep one section of a split  |
            \*---------------------------------------------------------------------------------------------------------------------------------------------------------------*/
            if (JobList[i].contains("\"><svg class"))
            {
                OpenRGBUpdateInfoPage::JobID = JobList[i].split("\"><svg class"[0])[0];
            }
        }
    }
    if (JobList.count() == 0)
    {
        CreateMsgDialog("JobList count = 0");
    }

    if (FoundPrefered)
    {
        UpdateProgressBar(true,"Finding latest commit artifact",70);
        QString ArtifactPageLink = "https://gitlab.com/" + Fork + "/OpenRGB/-/jobs/" + OpenRGBUpdateInfoPage::JobID + "/artifacts/browse/";

        if (WinExtensions)
        {
            ArtifactPageLink += "_build/";
        }

        QString ArtifactBrowsePage = GetPageSourceOrFile(ArtifactPageLink);

        QString FName;
        if (!WinExtensions)
        {
            FName = ContentCheck(ArtifactBrowsePage, {".AppImage</span>","AppImage\"><span>"},{0,1}) + ".AppImage";
        }
        else if (WinExtensions)
        {
            FName = ContentCheck(ArtifactBrowsePage, {"class=\"str-truncated\" href=\"", "\"><span>", "</span>"},{1,1,0});
        }

        if (FName.contains("PARSING FAIL"))
        {
            ContentMissing = true;
            UpdateProgressBar(false);
            CreateMsgDialog("Failed to find file name\nPlease contact @CoffeeIsLife on the discord");
            OpenRGBUpdateInfoPage::OwnedByUpdateButton = false;
            return;
        }

        UpdateProgressBar(true,"Downloading artifacts",80);

        QString FileStorageLocation = "";
        if (WinExtensions)
        {
            FileStorageLocation = qApp->applicationDirPath() + FName;
        }
        else
        {
            int SplitCount = qApp->applicationDirPath().count("/") - 3;
            FileStorageLocation  = qApp->applicationDirPath().split("/"[SplitCount])[0] + "/" + FName;
        }


        QString RawFileUrl = "https://gitlab.com/api/v4/projects/10582521/jobs/" + OpenRGBUpdateInfoPage::JobID + "/artifacts/";
        if (WinExtensions)
        {
            RawFileUrl += "_build/";
        }
        RawFileUrl += FName;
        QByteArray AppBuffer = GetPageSourceOrFile(RawFileUrl);
        UpdateProgressBar(true,"Writing info to file",90);
        /*--------------------------------------------------------------------------*\
        | Pack Recieved ByteArray into .zip/appimage file                            |
        | atm it defaults to the location that OpenRGB is already in                 |
        |                                                                            |
        | Will most likely need to be changed when the deb becomes more widely used  |
        \*--------------------------------------------------------------------------*/
        qInfo() << FileStorageLocation; // This can probably be left in so that people can find the locaton easier if they ran from the terminal
        QFile ORGBfile(FileStorageLocation);
        ORGBfile.open(QIODevice::WriteOnly);
        ORGBfile.write(AppBuffer);
        ORGBfile.close();
        /*------------------------------*\
        | Clear internal storage         |
        \*------------------------------*/
        ArtifactBrowsePage.clear();
        AppBuffer.clear();

        UpdateProgressBar(true,"Finished!",100);
        this->thread()->sleep(1);
        UpdateProgressBar(false);
    }
    else if (!FoundPrefered)
    {
        UpdateProgressBar(true, "Couldn't find build for current OS",0);
        this->thread()->sleep(1);
        UpdateProgressBar(false);
    }
    /*--------------------------------*\
    | Wipe the strings to save memory  |
    \*--------------------------------*/
    OpenRGBUpdatePipeLinePagestr.clear();
    PipeLinePage.clear();
    OpenRGBUpdateInfoPage::OwnedByUpdateButton = false;
}

void OpenRGBUpdateInfoPage::on_CheckButton_clicked()
{
    UpdateProgressBar(true,"Downloading Gitlab source for parsing",20);
    QString CommitPage = "https://gitlab.com/" + Fork + "/OpenRGB/-/commits/" + CheckBranch;
    /*-------------------------------------------------------------------------------------------------------------*\
    | The HTML structure of Gitlab's commit page is as follows;                                                     |
    | layout-page page-with-contextual-sidebar                                                                      |
    |   content-wrapper                                                                                             |
    |       container-fluid container-limited                                                                       |
    |           content                                                                                             |
    |               js-project-commits-show                                                                         |
    |                   project_10582521                                                                            |
    |                       list-unstyled content_list                                                              |
    |                           Commits row (Identified by a data-day string)                                       |
    |                               content-list commit-list flex-list                                              |
    |                                   commit flex-row js-toggle-container                                         |
    |                                       commit-detail flex-list                                                 |
    |                                           commit-content                                                      |
    |                                               commit-row-message item-title js-onboarding-commit-item         |
    |                                               ^^^ This Contails all of the commit details                     |
    \*-------------------------------------------------------------------------------------------------------------*/
    QString GitLabCommitPage = OpenRGBUpdateInfoPage::GetPageSourceOrFile(CommitPage);
    if ((GitLabCommitPage.contains("redirected")) || (GitLabCommitPage.contains("You are being")))
    {
        CreateMsgDialog("Usernames are Case sensitive\nPlease make sure you match their gitlab page exactly");
        UpdateProgressBar(false);
        OpenRGBUpdateInfoPage::ContentMissing = true;
        return;
    }

    /*--------------------------------------------------------------------------------------*\
    | Parsed returns 6 things in the same order                                              |
    | 1. Commit Date                                                                         |
    | 2. Commit Name (The message)                                                           |
    | 3. Commit Comments (Not alwaysed used so it may say "No Comment"                       |
    | 4. Commit ID (8 character ID)                                                          |
    | 5. Pipeline status (Either success or failed) Returns a string because I'm lazy        |
    | 6. Commit Hash (This isn't added to the data table as it isn't relevant to the user)   |
    \*--------------------------------------------------------------------------------------*/
    if (!OpenRGBUpdateInfoPage::OwnedByUpdateButton)
    {
        UpdateProgressBar(true,"Parsing list into data table",50);
    }

    std::vector<QStringList> Parsed = ParseCommitList(GitLabCommitPage);
    OpenRGBUpdateInfoPage::Parsed = Parsed;

    if ((Parsed[0][0] == "Parsing") && (Parsed[0][1] == "Error"))
    {
        UpdateProgressBar(false);
        ContentMissing = true;
        return;
    }
    /*---------------------------------------------------------------------*\
    |  Rather than using .clear just reassigning the rows should be fine    |
    \*---------------------------------------------------------------------*/
    if (!OpenRGBUpdateInfoPage::OwnedByUpdateButton)
    {
        UpdateProgressBar(true,"Adding info to table",70);
    }
    ui->GitCommitInfoTable->setRowCount(int(Parsed.size()));

    for (int i = 0; i < int(Parsed.size()); i++)
    {
        for (int SubI = 0; SubI < 5; SubI++)
        {
            QTableWidgetItem *TableInfo = new QTableWidgetItem(Parsed[i][SubI]);
            if ((SubI == 2) && (Parsed[i][SubI] != "No Comments"))
            {
                /*-----------------------------------------------------------------*\
                | Add Commit tooltip (In case the comment it too long for the box   |
                \*-----------------------------------------------------------------*/

                QString CommentToolTip = Parsed[i][SubI].replace("  ","\n");
                TableInfo->setToolTip(Parsed[i][SubI]);
            }
            ui->GitCommitInfoTable->setItem(i,SubI,TableInfo);
        }
    }
    if (!OpenRGBUpdateInfoPage::OwnedByUpdateButton)
    {
        UpdateProgressBar(true,"Done!",100);
        this->thread()->sleep(1);
        UpdateProgressBar(false);
    }

    /*----------------------------*\
    | More variable wiping         |
    \*----------------------------*/

    GitLabCommitPage.clear();
    Parsed.clear();
}
