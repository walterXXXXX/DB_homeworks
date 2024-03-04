#include <iostream>
#include <conio.h>
#include <pqxx/pqxx>
#include <Windows.h>
#pragma execution_character_set("utf-8")

class ClientsDB {
public:
	ClientsDB(std::string connection) {
		conn = new pqxx::connection(connection);
		createBD();
	}

	~ClientsDB() {
		delete conn;
	}

	// �����, ����������� �������� ������ �������
	void addClient(std::string firstName, std::string lastName, std::string email) {
		pqxx::work work(*conn);
		work.exec("INSERT INTO clients (firstname, lastname, email) VALUES('" +
			work.esc(firstName) + "', '" + work.esc(lastName) + "', '" + work.esc(email) + "');");
		work.commit();
	}

	// �����, ����������� �������� ������� ��� ������������� �������
	void addPhone(int ID, std::string phone) {
		pqxx::work work(*conn);
		work.exec("INSERT INTO phones (clientID, phone) VALUES('" +
			std::to_string(ID) + "', '" + work.esc(phone) + "');");
		work.commit();
	}

	// �����, ����������� �������� ������ � �������
	void updateClient(int ID, std::string firstName, std::string lastName, std::string email) {
		pqxx::work work(*conn);
		work.exec("UPDATE clients SET firstname = '" + work.esc(firstName) + 
			"', lastname = '" + work.esc(lastName) + "', email = '" + work.esc(email) +
			"' WHERE id = " + std::to_string(ID) + ";");
		work.commit();
	}

	// �����, ����������� ������� ������� � ������������� �������
	void deletePhone(int ID, std::string phone) {
		pqxx::work work(*conn);
		work.exec("DELETE FROM phones WHERE clientid = " + std::to_string(ID) + 
		" AND phone = '" + work.esc(phone) + "';");
		work.commit();
	}

	// �����, ����������� ������� ������������� �������
	void deleteClient(int ID) {
		pqxx::work work(*conn);
		work.exec("DELETE FROM phones WHERE clientid = " + std::to_string(ID));
		work.exec("DELETE FROM clients WHERE id = " + std::to_string(ID));
		work.commit();
	}

	// ����� ��������� ��� ������ ��
	void clearDB() {
		pqxx::work work(*conn);
		work.exec("DELETE FROM phones");
		work.exec("DELETE FROM clients");
		work.commit();
	}

	// �����, ����������� ����� ������� �� ��� ������ � �����, �������, email ��� ��������
	int getClientID(std::string firstName, std::string lastName, std::string email) {
		pqxx::work work(*conn);
		int result = work.query_value<int>("SELECT id FROM clients WHERE firstname = '" + 
			work.esc(firstName) + "' AND lastname = '" + work.esc(lastName) + 
			"' AND email = '" + work.esc(email) + "';");
		work.commit();
		return result;
	}

	int getClientID(std::string phone) {
		pqxx::work work(*conn);
		int result = work.query_value<int>("SELECT clientid FROM phones WHERE phone = '" +
			work.esc(phone) + "';");
		work.commit();
		return result;
	}

	// ����� ������ �� ����� ���������� � �������
	void printClient(int ID) {
		pqxx::work work(*conn);
		auto clientData = work.query<std::string, std::string, std::string>(
			"SELECT firstname, lastname, email FROM clients WHERE id = '" + std::to_string(ID) + "';");
		std::cout << "������ � ID=" << ID << ": " <<
			std::get<0>(*clientData.begin()) << " " << std::get<1>(*clientData.begin()) << std::endl;
		std::cout << "email: " << std::get<2>(*clientData.begin()) << std::endl;
		auto phones = work.query<std::string>(
			"SELECT phone FROM phones WHERE clientid = " + std::to_string(ID) + ";");
		std::cout << "���.: " ;
		for (const auto& ph : phones) {
			std::cout << std::get<0>(ph) << " ";
		}
		std::cout << std::endl << std::endl;
		work.commit();
	}

	// ����� ������ �� ����� ����������� ���� ������
	void printDB() {
		pqxx::work work(*conn);
		auto clients = work.query<int, std::string, std::string, std::string>(
			"SELECT * from clients;");
		std::cout << "�� ��������" << std::endl;
		if (clients.begin() == clients.end()) {
			std::cout << "� �� ��� �������" << std::endl;
			return;
		}
		for (const auto& cl : clients) {
			int ID = std::get<0>(cl);
			std::cout << "������ � ID=" << ID << ": ";
			std::cout << std::get<1>(cl) << " " << std::get<2>(cl) << std::endl;
			std::cout << "email: " << std::get<3>(cl) << std::endl;
			std::cout << "���.: ";
			auto phones = work.query<std::string>(
				"SELECT phone FROM phones WHERE clientid = " + std::to_string(ID) + ";");
			for (const auto& ph : phones) {
				std::cout << std::get<0>(ph) << " ";
			}
			std::cout << std::endl << std::endl;
		}
		work.commit();
	}

private:
	pqxx::connection* conn;
	// �����, ��������� ��������� �� (�������)
	void createBD() {
		pqxx::work work(*conn);
		work.exec("CREATE TABLE IF NOT EXISTS clients (id SERIAL PRIMARY KEY, "
			"firstname VARCHAR(64) NOT NULL, lastname VARCHAR(64) NOT NULL, "
			"email VARCHAR(256) NOT NULL UNIQUE)");
		work.exec("CREATE TABLE IF NOT EXISTS phones (id SERIAL PRIMARY KEY, "
			"clientID INT NOT NULL, phone VARCHAR(64) NOT NULL UNIQUE, "
			"FOREIGN KEY (clientID) REFERENCES clients(id))");
		work.commit();
	}

};

int main() {
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);

	try {
		std::cout << "������������ ������ � �� ��������" << std::endl;
		
		std::cout << "1. �������� ����� �� ��� ������� ��� ��������� �����, � ������� ������� �� �� �����: \n\n";
		ClientsDB cdb("host=localhost "
				"port=5432 "
				"dbname=Lesson05DB "
				"user=postgres "
				"password=my_password");
		cdb.clearDB();
		cdb.printDB();
		std::cout << "������� ����� ������� ��� �����������...\n"; _getch();

		std::cout << "\n2. ������� ��������� ����� �������� � ������� �� �� �����: \n\n";
		cdb.addClient("����", "������", "ivanov@mail.ru");
		cdb.addPhone(cdb.getClientID("����", "������", "ivanov@mail.ru"), "+79991111111");
		cdb.addClient("����", "������", "petrov@mail.ru");
		cdb.addPhone(cdb.getClientID("����", "������", "petrov@mail.ru"), "+79992222222");
		cdb.addClient("����", "������", "sidorov@yandex.ru");
		cdb.addPhone(cdb.getClientID("����", "������", "sidorov@yandex.ru"), "+79993333333");
		cdb.printDB();
		std::cout << "������� ����� ������� ��� �����������...\n"; _getch();

		std::cout << "\n3. �������� �������� � ����� ���������� ������� � ������� ����������� ���������� � ���: \n\n";
		cdb.updateClient(cdb.getClientID("+79993333333"), "�������", "�������", "sidorov@yandex.ru");
		cdb.printClient(cdb.getClientID("+79993333333"));
		std::cout << "������� ����� ������� ��� �����������...\n"; _getch();

		std::cout << "\n4. ������� ���. ������ ��������� ������� ������� �������� � ������� ����������� ���������� � ���: \n\n";
		cdb.addPhone(cdb.getClientID("�������", "�������", "sidorov@yandex.ru"), "+79994444444");
		cdb.addPhone(cdb.getClientID("�������", "�������", "sidorov@yandex.ru"), "+79995555555");
		cdb.printClient(cdb.getClientID("+79995555555"));
		std::cout << "������� ����� ������� ��� �����������...\n"; _getch();

		std::cout << "\n5. ������ ������� ������� ����� ������� � ������� ����������� ���������� � ���: \n\n";
		cdb.deletePhone(cdb.getClientID("����", "������", "petrov@mail.ru"), "+79992222222");
		cdb.printClient(cdb.getClientID("����", "������", "petrov@mail.ru"));
		std::cout << "������� ����� ������� ��� �����������...\n"; _getch();

		std::cout << "\n6. ������ ������� ����� ������� �� �� � ������� ����������� �� �� �����: \n\n";
		cdb.deleteClient(cdb.getClientID("����", "������", "ivanov@mail.ru"));
		cdb.printDB();
		std::cout << "������� ����� ������� ��� �����������...\n"; _getch();

	}
	catch (const std::exception& ex) {
		std::cout << "Exception:\n" << ex.what() << std::endl;
	}
}