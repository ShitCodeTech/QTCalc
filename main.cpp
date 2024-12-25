#include <QApplication>
#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QString>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QJSEngine>
#include <QtMath>
#include <QKeyEvent>
#include <functional>

class Calculator : public QWidget {
    Q_OBJECT

public:
    Calculator(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("Advanced Calculator");
        setFixedSize(400, 500);

        // Create menu bar for theme toggle
        QMenuBar *menuBar = new QMenuBar(this);
        QMenu *viewMenu = new QMenu("View", this);
        QAction *toggleThemeAction = new QAction("Toggle Theme", this);
        viewMenu->addAction(toggleThemeAction);
        menuBar->addMenu(viewMenu);
        connect(toggleThemeAction, &QAction::triggered, this, &Calculator::toggleTheme);

        // Set dark theme by default
        isDarkTheme = true;
        applyTheme();

        // Create display
        display = new QLineEdit(this);
        display->setReadOnly(true);
        display->setAlignment(Qt::AlignRight);
        display->setStyleSheet("background-color: #2E2E2E; border-color: #FFFFFF; border: 3px; padding: 10px; font-size: 24px;");

        // Create buttons
        QStringList buttonLabels = {
            "C", "Back", "=", "/",
            "7", "8", "9", "*",
            "4", "5", "6", "-",
            "1", "2", "3", "+",
            "0", "00", ".", "^",
            "cos", "sin", "tan", "cot"
        };

        QGridLayout *gridLayout = new QGridLayout;

        for (int i = 0; i < buttonLabels.size(); ++i) {
            if (buttonLabels[i].isEmpty()) {
                gridLayout->addWidget(new QWidget(this), i / 4, i % 4); // Empty space
            } else {
                QPushButton *button = new QPushButton(buttonLabels[i], this);
                button->setStyleSheet(
                    "background-color: #3A3A3A; border: none; padding: 10px; font-size: 16px;"
                    "border-radius: 5px; color: #FFFFFF;"
                );
                connect(button, &QPushButton::clicked, this, &Calculator::onButtonClicked);
                gridLayout->addWidget(button, i / 4, i % 4);
            }
        }

        // Main layout
        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->setMenuBar(menuBar);
        mainLayout->addWidget(display);
        mainLayout->addLayout(gridLayout);
        setLayout(mainLayout);
    }

protected:
    void keyPressEvent(QKeyEvent *event) override {
        if (display->text() == "Error") {
            display->clear();
        }

        switch (event->key()) {
        case Qt::Key_0: case Qt::Key_1: case Qt::Key_2: case Qt::Key_3: case Qt::Key_4:
        case Qt::Key_5: case Qt::Key_6: case Qt::Key_7: case Qt::Key_8: case Qt::Key_9:
            display->setText(display->text() + event->text());
            break;
        case Qt::Key_Plus:
            display->setText(display->text() + "+");
            break;
        case Qt::Key_Minus:
            display->setText(display->text() + "-");
            break;
        case Qt::Key_Asterisk:
            display->setText(display->text() + "*");
            break;
        case Qt::Key_Slash:
            display->setText(display->text() + "/");
            break;
        case Qt::Key_Period:
            display->setText(display->text() + ".");
            break;
        case Qt::Key_Backspace:
            display->setText(display->text().chopped(1));
            break;
        case Qt::Key_Enter: case Qt::Key_Return:
            calculateResult();
            break;
        case Qt::Key_C:
            display->clear();
            break;
        default:
            QWidget::keyPressEvent(event);
        }
    }

private slots:
    void onButtonClicked() {
        QPushButton *button = qobject_cast<QPushButton *>(sender());
        if (!button)
            return;

        QString buttonText = button->text();

        if (display->text() == "Error" && buttonText != "C") {
            display->clear();
        }

        if (buttonText == "=") {
            calculateResult();
        } else if (buttonText == "C") {
            display->clear();
        } else if (buttonText == "Back") {
            QString text = display->text();
            text.chop(1);
            display->setText(text);
        } else if (buttonText == "cos") {
            applyTrigonometricFunction(qCos);
        } else if (buttonText == "sin") {
            applyTrigonometricFunction(qSin);
        } else if (buttonText == "tan") {
            applyTrigonometricFunction(qTan);
        } else if (buttonText == "cot") {
            applyTrigonometricFunction([](double value) { return 1.0 / qTan(value); });
        } else if (buttonText == "^") {
            display->setText(display->text() + "^");
        } else if (buttonText == "!") {
            calculateFactorial();
        } else {
            display->setText(display->text() + buttonText);
        }
    }

    void toggleTheme() {
        isDarkTheme = !isDarkTheme;
        applyTheme();
    }

private:
    QLineEdit *display;
    bool isDarkTheme;  

    void applyTheme() {
        if (isDarkTheme) {
            setStyleSheet("background-color: #2E2E2E; color: #FFFFFF;");
    	} else {
       	    setStyleSheet("background-color: #FFFFFF; color: #000000;");
    	}
    }



    void calculateResult() {
        QString expression = display->text();
        expression.replace("^", "**"); // Replace ^ with ** for power operation
        QJSEngine engine;
        QJSValue result = engine.evaluate(expression);

        if (result.isError()) {
            display->setText("Error");
        } else {
            display->setText(result.toString());
        }
    }

    void applyTrigonometricFunction(std::function<double(double)> func) {
        bool ok;
        double value = display->text().toDouble(&ok);
        if (ok) {
            double result = func(value);
            display->setText(QString::number(result));
        } else {
            display->setText("Error");
        }
    }

    void calculateFactorial() {
        bool ok;
        int number = display->text().toInt(&ok);
        if (ok && number >= 0) {
            display->setText(QString::number(factorial(number)));
        } else {
            display->setText("Error");
        }
    }

    int factorial(int n) {
        return (n == 0 || n == 1) ? 1 : n * factorial(n - 1);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    Calculator calculator;
    calculator.show();

    return app.exec();
}

#include "main.moc"

